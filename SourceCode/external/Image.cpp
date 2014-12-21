#include "stdafx.h"
#include "Image.h"
#include "Configuration.h"
#include "external/ZipUtils.h"
#include "external/S3tc.h"
#include "jpeglib.h"
#include "png.h"
#include "tiffio.h"
#include "decode.h"
#include "external/TgaLib.h"
#include "external/Atitc.h"
#include "external/etc1/etc1.h"
#include "pvr.h"

#pragma warning(disable:4611) // for load png warning

#define CC_GL_ATC_RGB_AMD                                          0x8C92
#define CC_GL_ATC_RGBA_EXPLICIT_ALPHA_AMD                          0x8C93
#define CC_GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD                      0x87EE

namespace
{
    struct SErrorMgr
    {
        struct jpeg_error_mgr pub;    /* "public" fields */
        jmp_buf setjmp_buffer;    /* for return to caller */
    };
    typedef SErrorMgr* SErrorMgrPtr;
    METHODDEF(void) ErrorExit(j_common_ptr cinfo)
    {
        /* cinfo->err really points to a MyErrorMgr struct, so coerce pointer */
            SErrorMgrPtr myerr = (SErrorMgrPtr) cinfo->err;
        
            /* Always display the message. */
            /* We could postpone this until after returning, if we chose. */
            /* internal message function cann't show error message in some platforms, so we rewrite it here.
             * edit it if has version confilict.
             */
            //(*cinfo->err->output_message) (cinfo);
            char buffer[JMSG_LENGTH_MAX];
            (*cinfo->err->format_message) (cinfo, buffer);
            BEATS_PRINT(_T("jpeg error: %s"), buffer);
        
            /* Return control to the setjmp point */
            longjmp(myerr->setjmp_buffer, 1);
    }
}
namespace
{
    void PngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        TImageSource* isource = (TImageSource*)png_get_io_ptr(png_ptr);

        if((int)(isource->offset + length) <= isource->size)
        {
            memcpy(data, isource->data+isource->offset, length);
            isource->offset += length;
        }
        else
        {
            png_error(png_ptr, "pngReaderCallback failed");
        }
    }
}

PixelFormat getDevicePixelFormat(PixelFormat format)
{
    switch (format) 
    {
    case PixelFormat::PVRTC4:
    case PixelFormat::PVRTC4A:
    case PixelFormat::PVRTC2:
    case PixelFormat::PVRTC2A:
        if (!CConfiguration::GetInstance()->SupportsPVRTC())
        {
            format = PixelFormat::RGBA8888;
        }
        break;
    case PixelFormat::ETC:
        if (!CConfiguration::GetInstance()->SupportsETC())
        {
            format = PixelFormat::RGB888;
        }
        break;
    default:
        break;
    }
    return format;
}

namespace
{
    static tmsize_t tiffReadProc(thandle_t fd, void* buf, tmsize_t size)
    {
        TImageSource* isource = (TImageSource*)fd;
        uint8* ma;
        uint64 mb;
        unsigned long n;
        unsigned long o;
        tmsize_t p;
        ma=(uint8*)buf;
        mb=size;
        p=0;
        while (mb>0)
        {
            n=0x80000000UL;
            if ((uint64)n>mb)
                n=(unsigned long)mb;


            if((int)(isource->offset + n) <= isource->size)
            {
                memcpy(ma, isource->data+isource->offset, n);
                isource->offset += n;
                o = n;
            }
            else
            {
                return 0;
            }

            ma+=o;
            mb-=o;
            p+=o;
            if (o!=n)
            {
                break;
            }
        }
        return p;
    }

    static tmsize_t tiffWriteProc(thandle_t fd, void* buf, tmsize_t size)
    {
        BEYONDENGINE_UNUSED_PARAM(fd);
        BEYONDENGINE_UNUSED_PARAM(buf);
        BEYONDENGINE_UNUSED_PARAM(size);
        return 0;
    }


    static uint64 tiffSeekProc(thandle_t fd, uint64 off, int whence)
    {
        TImageSource* isource = (TImageSource*)fd;
        uint64 ret = 0;
        do
        {
            if (whence == SEEK_SET)
            {
                if(off >= (uint64)isource->size)
                {
                    break;
                }
                ret = isource->offset = (uint32)off;
            }
            else if (whence == SEEK_CUR)
            {
                if(isource->offset + off >= (uint64)isource->size)
                {
                    break;
                }
                ret = isource->offset += (uint32)off;
            }
            else if (whence == SEEK_END)
            {
                if(off >= (uint64)isource->size)
                {
                    break;
                }
                ret = isource->offset = (uint32)(isource->size-1 - off);
            }
            else
            {
                if(off >= (uint64)isource->size)
                {
                    break;
                }
                ret = isource->offset = (uint32)off;
            }
        } while (0);

        return ret;
    }

    static uint64 tiffSizeProc(thandle_t fd)
    {
        TImageSource* pImageSrc = (TImageSource*)fd;
        return pImageSrc->size;
    }

    static int tiffCloseProc(thandle_t fd)
    {
        BEYONDENGINE_UNUSED_PARAM(fd);
        return 0;
    }

    static int tiffMapProc(thandle_t fd, void** pbase, toff_t* psize)
    {
        BEYONDENGINE_UNUSED_PARAM(fd);
        BEYONDENGINE_UNUSED_PARAM(pbase);
        BEYONDENGINE_UNUSED_PARAM(psize);
        return 0;
    }

    static void tiffUnmapProc(thandle_t fd, void* base, toff_t size)
    {
        BEYONDENGINE_UNUSED_PARAM(fd);
        BEYONDENGINE_UNUSED_PARAM(base);
        BEYONDENGINE_UNUSED_PARAM(size);
    }
}

namespace
{
    static const int PVR_TEXTURE_FLAG_TYPE_MASK = 0xff;

    static bool _PVRHaveAlphaPremultiplied = false;

    // Values taken from PVRTexture.h from http://www.imgtec.com
    enum class PVR2TextureFlag
    {
        Mipmap = (1 << 8),        // has mip map levels
        Twiddle = (1 << 9),        // is twiddled
        Bumpmap = (1 << 10),       // has normals encoded for a bump map
        Tiling = (1 << 11),       // is bordered for tiled pvr
        Cubemap = (1 << 12),       // is a cubemap/skybox
        FalseMipCol = (1 << 13),       // are there false colored MIP levels
        Volume = (1 << 14),       // is this a volume texture
        Alpha = (1 << 15),       // v2.1 is there transparency info in the texture
        VerticalFlip = (1 << 16),       // v2.1 is the texture vertically flipped
    };

    enum class PVR3TextureFlag
    {
        PremultipliedAlpha = (1 << 1)    // has premultiplied alpha
    };

    static const char gPVRTexIdentifier[5] = "PVR!";

    // v2
    enum class PVR2TexturePixelFormat : unsigned char
    {
        RGBA4444 = 0x10,
        RGBA5551,
        RGBA8888,
        RGB565,
        RGB555,          // unsupported
        RGB888,
        I8,
        AI88,
        PVRTC2BPP_RGBA,
        PVRTC4BPP_RGBA,
        BGRA8888,
        A8,
    };

    // v3
    enum class PVR3TexturePixelFormat : uint64_t
    {
        PVRTC2BPP_RGB = 0ULL,
        PVRTC2BPP_RGBA = 1ULL,
        PVRTC4BPP_RGB = 2ULL,
        PVRTC4BPP_RGBA = 3ULL,
        PVRTC2_2BPP_RGBA = 4ULL,
        PVRTC2_4BPP_RGBA = 5ULL,
        ETC1 = 6ULL,
        DXT1 = 7ULL,
        DXT2 = 8ULL,
        DXT3 = 9ULL,
        DXT4 = 10ULL,
        DXT5 = 11ULL,
        BC1 = 7ULL,
        BC2 = 9ULL,
        BC3 = 11ULL,
        BC4 = 12ULL,
        BC5 = 13ULL,
        BC6 = 14ULL,
        BC7 = 15ULL,
        UYVY = 16ULL,
        YUY2 = 17ULL,
        BW1bpp = 18ULL,
        R9G9B9E5 = 19ULL,
        RGBG8888 = 20ULL,
        GRGB8888 = 21ULL,
        ETC2_RGB = 22ULL,
        ETC2_RGBA = 23ULL,
        ETC2_RGBA1 = 24ULL,
        EAC_R11_Unsigned = 25ULL,
        EAC_R11_Signed = 26ULL,
        EAC_RG11_Unsigned = 27ULL,
        EAC_RG11_Signed = 28ULL,

        BGRA8888 = 0x0808080861726762ULL,
        RGBA8888 = 0x0808080861626772ULL,
        RGBA4444 = 0x0404040461626772ULL,
        RGBA5551 = 0x0105050561626772ULL,
        RGB565 = 0x0005060500626772ULL,
        RGB888 = 0x0008080800626772ULL,
        A8 = 0x0000000800000061ULL,
        L8 = 0x000000080000006cULL,
        LA88 = 0x000008080000616cULL,
    };


    // v2
    typedef const std::map<PVR2TexturePixelFormat, PixelFormat> _pixel2_formathash;

    static const _pixel2_formathash::value_type v2_pixel_formathash_value[] =
    {
        _pixel2_formathash::value_type(PVR2TexturePixelFormat::BGRA8888, PixelFormat::BGRA8888),
        _pixel2_formathash::value_type(PVR2TexturePixelFormat::RGBA8888, PixelFormat::RGBA8888),
        _pixel2_formathash::value_type(PVR2TexturePixelFormat::RGBA4444, PixelFormat::RGBA4444),
        _pixel2_formathash::value_type(PVR2TexturePixelFormat::RGBA5551,        PixelFormat::RGB5A1),
        _pixel2_formathash::value_type(PVR2TexturePixelFormat::RGB565,      PixelFormat::RGB565),
        _pixel2_formathash::value_type(PVR2TexturePixelFormat::RGB888,      PixelFormat::RGB888),
        _pixel2_formathash::value_type(PVR2TexturePixelFormat::A8,          PixelFormat::A8),
        _pixel2_formathash::value_type(PVR2TexturePixelFormat::I8,          PixelFormat::I8),
        _pixel2_formathash::value_type(PVR2TexturePixelFormat::AI88, PixelFormat::AI88),

        _pixel2_formathash::value_type(PVR2TexturePixelFormat::PVRTC2BPP_RGBA, PixelFormat::PVRTC2A),
        _pixel2_formathash::value_type(PVR2TexturePixelFormat::PVRTC4BPP_RGBA, PixelFormat::PVRTC4A),
    };

    static const int PVR2_MAX_TABLE_ELEMENTS = sizeof(v2_pixel_formathash_value) / sizeof(v2_pixel_formathash_value[0]);
    static const _pixel2_formathash v2_pixel_formathash(v2_pixel_formathash_value, v2_pixel_formathash_value + PVR2_MAX_TABLE_ELEMENTS);

    // v3
    typedef const std::map<PVR3TexturePixelFormat, PixelFormat> _pixel3_formathash;
    static _pixel3_formathash::value_type v3_pixel_formathash_value[] =
    {
        _pixel3_formathash::value_type(PVR3TexturePixelFormat::BGRA8888, PixelFormat::BGRA8888),
        _pixel3_formathash::value_type(PVR3TexturePixelFormat::RGBA8888, PixelFormat::RGBA8888),
        _pixel3_formathash::value_type(PVR3TexturePixelFormat::RGBA4444, PixelFormat::RGBA4444),
        _pixel3_formathash::value_type(PVR3TexturePixelFormat::RGBA5551, PixelFormat::RGB5A1),
        _pixel3_formathash::value_type(PVR3TexturePixelFormat::RGB565, PixelFormat::RGB565),
        _pixel3_formathash::value_type(PVR3TexturePixelFormat::RGB888, PixelFormat::RGB888),
        _pixel3_formathash::value_type(PVR3TexturePixelFormat::A8, PixelFormat::A8),
        _pixel3_formathash::value_type(PVR3TexturePixelFormat::L8, PixelFormat::I8),
        _pixel3_formathash::value_type(PVR3TexturePixelFormat::LA88, PixelFormat::AI88),

        _pixel3_formathash::value_type(PVR3TexturePixelFormat::PVRTC2BPP_RGB, PixelFormat::PVRTC2),
        _pixel3_formathash::value_type(PVR3TexturePixelFormat::PVRTC2BPP_RGBA, PixelFormat::PVRTC2A),
        _pixel3_formathash::value_type(PVR3TexturePixelFormat::PVRTC4BPP_RGB, PixelFormat::PVRTC4),
        _pixel3_formathash::value_type(PVR3TexturePixelFormat::PVRTC4BPP_RGBA, PixelFormat::PVRTC4A),

        _pixel3_formathash::value_type(PVR3TexturePixelFormat::ETC1, PixelFormat::ETC),
    };

    static const int PVR3_MAX_TABLE_ELEMENTS = sizeof(v3_pixel_formathash_value) / sizeof(v3_pixel_formathash_value[0]);

    static const _pixel3_formathash v3_pixel_formathash(v3_pixel_formathash_value, v3_pixel_formathash_value + PVR3_MAX_TABLE_ELEMENTS);

    typedef struct _PVRTexHeader
    {
        unsigned int headerLength;
        unsigned int height;
        unsigned int width;
        unsigned int numMipmaps;
        unsigned int flags;
        unsigned int dataLength;
        unsigned int bpp;
        unsigned int bitmaskRed;
        unsigned int bitmaskGreen;
        unsigned int bitmaskBlue;
        unsigned int bitmaskAlpha;
        unsigned int pvrTag;
        unsigned int numSurfs;
    } PVRv2TexHeader;

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
    typedef struct
    {
        uint32_t version;
        uint32_t flags;
        uint64_t pixelFormat;
        uint32_t colorSpace;
        uint32_t channelType;
        uint32_t height;
        uint32_t width;
        uint32_t depth;
        uint32_t numberOfSurfaces;
        uint32_t numberOfFaces;
        uint32_t numberOfMipmaps;
        uint32_t metadataLength;
#ifdef _MSC_VER
    } PVRv3TexHeader;
#pragma pack(pop)
#else
    } __attribute__((packed)) PVRv3TexHeader;
#endif

    bool testFormatForPvr2TCSupport(PVR2TexturePixelFormat /*format*/)
    {
        return true;
    }

    bool testFormatForPvr3TCSupport(PVR3TexturePixelFormat format)
    {
        bool bRet = false;
        switch (format) 
        {
        case PVR3TexturePixelFormat::DXT1:
        case PVR3TexturePixelFormat::DXT3:
        case PVR3TexturePixelFormat::DXT5:
            bRet = CConfiguration::GetInstance()->SupportsS3TC();
            break;
        case PVR3TexturePixelFormat::BGRA8888:
            bRet = CConfiguration::GetInstance()->SupportsBGRA8888();
            break;
        case PVR3TexturePixelFormat::PVRTC2BPP_RGB:
        case PVR3TexturePixelFormat::PVRTC2BPP_RGBA:
        case PVR3TexturePixelFormat::PVRTC4BPP_RGB:
        case PVR3TexturePixelFormat::PVRTC4BPP_RGBA:
        case PVR3TexturePixelFormat::ETC1:
        case PVR3TexturePixelFormat::RGBA8888:
        case PVR3TexturePixelFormat::RGBA4444:
        case PVR3TexturePixelFormat::RGBA5551:
        case PVR3TexturePixelFormat::RGB565:
        case PVR3TexturePixelFormat::RGB888:
        case PVR3TexturePixelFormat::A8:
        case PVR3TexturePixelFormat::L8:
        case PVR3TexturePixelFormat::LA88:
            bRet = true;
            break;
        default:
            bRet = false;
            break;
        }
        return bRet;
    }
}

//struct and data for s3tc(dds) struct
namespace
{
    struct DDColorKey
    {
        uint32_t colorSpaceLowValue;
        uint32_t colorSpaceHighValue;
    };

    struct DDSCaps
    {
        uint32_t caps;
        uint32_t caps2;
        uint32_t caps3;
        uint32_t caps4;
    };

    struct DDPixelFormat
    {
        uint32_t size;
        uint32_t flags;
        uint32_t fourCC;
        uint32_t RGBBitCount;
        uint32_t RBitMask;
        uint32_t GBitMask;
        uint32_t BBitMask;
        uint32_t ABitMask;
    };


    struct DDSURFACEDESC2
    {
        uint32_t size;
        uint32_t flags;
        uint32_t height;
        uint32_t width;

        union
        {
            uint32_t pitch;
            uint32_t linearSize;
        } DUMMYUNIONNAMEN1;

        union
        {
            uint32_t backBufferCount;
            uint32_t depth;
        } DUMMYUNIONNAMEN5;

        union
        {
            uint32_t mipMapCount;
            uint32_t refreshRate;
            uint32_t srcVBHandle;
        } DUMMYUNIONNAMEN2;

        uint32_t alphaBitDepth;
        uint32_t reserved;
        uint32_t surface;

        union
        {
            DDColorKey ddckCKDestOverlay;
            uint32_t emptyFaceColor;
        } DUMMYUNIONNAMEN3;

        DDColorKey ddckCKDestBlt;
        DDColorKey ddckCKSrcOverlay;
        DDColorKey ddckCKSrcBlt;

        union
        {
            DDPixelFormat ddpfPixelFormat;
            uint32_t FVF;
        } DUMMYUNIONNAMEN4;

        DDSCaps ddsCaps;
        uint32_t textureStage;
    } ;

#pragma pack(push,1)

    struct S3TCTexHeader
    {
        char fileCode[4];
        DDSURFACEDESC2 ddsd;
    };

#pragma pack(pop)

    static const uint32_t makeFourCC(char ch0, char ch1, char ch2, char ch3)
    {
        const uint32_t fourCC = ((uint32_t)(char)(ch0) | ((uint32_t)(char)(ch1) << 8) | ((uint32_t)(char)(ch2) << 16) | ((uint32_t)(char)(ch3) << 24 ));
        return fourCC;
    }

}

//struct and data for atitc(ktx) struct
namespace
{
    struct ATITCTexHeader
    {
        //HEADER
        char identifier[12];
        uint32_t endianness;
        uint32_t glType;
        uint32_t glTypeSize;
        uint32_t glFormat;
        uint32_t glInternalFormat;
        uint32_t glBaseInternalFormat;
        uint32_t pixelWidth;
        uint32_t pixelHeight;
        uint32_t pixelDepth;
        uint32_t numberOfArrayElements;
        uint32_t numberOfFaces;
        uint32_t numberOfMipmapLevels;
        uint32_t bytesOfKeyValueData;
    };
}

typedef PixelFormatInfoMap::value_type PixelFormatInfoMapValue;
static const PixelFormatInfoMapValue TexturePixelFormatInfoTablesValue[] =
{
    PixelFormatInfoMapValue(PixelFormat::BGRA8888, PixelFormatInfo(GL_BGRA, GL_BGRA, GL_UNSIGNED_BYTE, 32, false, true)),
    PixelFormatInfoMapValue(PixelFormat::RGBA8888, PixelFormatInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, 32, false, true)),
    PixelFormatInfoMapValue(PixelFormat::RGBA4444, PixelFormatInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, 16, false, true)),
    PixelFormatInfoMapValue(PixelFormat::RGB5A1, PixelFormatInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, 16, false, true)),
    PixelFormatInfoMapValue(PixelFormat::RGB565, PixelFormatInfo(GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 16, false, false)),
    PixelFormatInfoMapValue(PixelFormat::RGB888, PixelFormatInfo(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, 24, false, false)),
    PixelFormatInfoMapValue(PixelFormat::A8, PixelFormatInfo(GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE, 8, false, false)),
    PixelFormatInfoMapValue(PixelFormat::I8, PixelFormatInfo(GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE, 8, false, false)),
    PixelFormatInfoMapValue(PixelFormat::AI88, PixelFormatInfo(GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 16, false, true)),

#ifdef GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
    PixelFormatInfoMapValue(PixelFormat::PVRTC2, PixelFormatInfo(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 2, true, false)),
    PixelFormatInfoMapValue(PixelFormat::PVRTC2A, PixelFormatInfo(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 2, true, true)),
    PixelFormatInfoMapValue(PixelFormat::PVRTC4, PixelFormatInfo(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 4, true, false)),
    PixelFormatInfoMapValue(PixelFormat::PVRTC4A, PixelFormatInfo(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 4, true, true)),
#endif

#ifdef GL_ETC1_RGB8_OES
    PixelFormatInfoMapValue(PixelFormat::ETC, PixelFormatInfo(GL_ETC1_RGB8_OES, 0xFFFFFFFF, 0xFFFFFFFF, 4, true, false)),
#endif

#ifdef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
    PixelFormatInfoMapValue(PixelFormat::S3TC_DXT1, PixelFormatInfo(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0xFFFFFFFF, 0xFFFFFFFF, 4, true, false)),
#endif

#ifdef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
    PixelFormatInfoMapValue(PixelFormat::S3TC_DXT3, PixelFormatInfo(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0xFFFFFFFF, 0xFFFFFFFF, 8, true, false)),
#endif

#ifdef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
    PixelFormatInfoMapValue(PixelFormat::S3TC_DXT5, PixelFormatInfo(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0xFFFFFFFF, 0xFFFFFFFF, 8, true, false)),
#endif

#ifdef GL_ATC_RGB_AMD
    PixelFormatInfoMapValue(PixelFormat::ATC_RGB, PixelFormatInfo(GL_ATC_RGB_AMD,
    0xFFFFFFFF, 0xFFFFFFFF, 4, true, false)),
#endif

#ifdef GL_ATC_RGBA_EXPLICIT_ALPHA_AMD
    PixelFormatInfoMapValue(PixelFormat::ATC_EXPLICIT_ALPHA, PixelFormatInfo(GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,
    0xFFFFFFFF, 0xFFFFFFFF, 8, true, false)),
#endif

#ifdef GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD
    PixelFormatInfoMapValue(PixelFormat::ATC_INTERPOLATED_ALPHA, PixelFormatInfo(GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,
    0xFFFFFFFF, 0xFFFFFFFF, 8, true, false)),
#endif
};
const PixelFormatInfoMap CImage::_pixelFormatInfoTables(TexturePixelFormatInfoTablesValue,
                                                          TexturePixelFormatInfoTablesValue + sizeof(TexturePixelFormatInfoTablesValue) / sizeof(TexturePixelFormatInfoTablesValue[0]));
CImage::CImage()
{
    m_data = NULL;
    m_dataLen = 0;
    m_width = 0;
    m_height = 0;
    m_fileType = EFormat::eF_UNKOWN;
    m_renderFormat = PixelFormat::NONE;
    m_preMulti = false;
    m_numberOfMipmaps = 0;
    m_hasPremultipliedAlpha = true;
    memset(&m_mipmaps, 0, sizeof(m_mipmaps));
}

CImage::~CImage()
{
    BEATS_SAFE_DELETE_ARRAY( m_data );
}



const unsigned char* CImage::GetData() const
{
    return m_data;
}

void CImage::SetData(PixelFormat renderFormat, unsigned char* pData, ssize_t len)
{
    if (m_data != nullptr)
    {
        BEATS_SAFE_DELETE_ARRAY(m_data);
    }
    m_data = pData;
    m_dataLen = len;
    m_renderFormat = renderFormat;
}

ssize_t CImage::GetDataLength() const
{
    return m_dataLen;
}

CImage::EFormat CImage::GetFileType() const
{
    return m_fileType;
}

PixelFormat CImage::GetRenderFormat() const
{
    return m_renderFormat;
}

int CImage::GetWidth() const
{
    return m_width;
}

int CImage::GetHeight() const
{
    return m_height;
}

bool CImage::IsPremultipliedAlpha() const
{
    return m_preMulti;
}

int CImage::GetNumberOfMipmaps() const
{
    return m_numberOfMipmaps;
}

const TMipmapInfo* CImage::GetMipmaps() const
{
    return m_mipmaps;
}

bool CImage::HasPremultipliedAlpha() const
{
    return m_hasPremultipliedAlpha;
}

bool CImage::InitWithRawData( const unsigned char * data, ssize_t /*dataLen*/, int width, int height, int /*bitsPerComponent*/, const TCHAR* filename, bool preMulti /*= false*/ )
{
    bool bRet = false;
    do 
    {
        if(0 == width || 0 == height)
        {
            break;
        }
        m_height   = height;
        m_width    = width;
        m_preMulti = preMulti;
        m_renderFormat = PixelFormat::RGBA8888;

        // only RGBA8888 supported
        int bytesPerComponent = 4;
        m_dataLen = height * width * bytesPerComponent;
        m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);
        if ( m_data )
        {
            memcpy( m_data, data, m_dataLen );
            bRet = true;
        }
    } while (0);

    return bRet;
}

int CImage::GetBitPerPixel()
{
    return _pixelFormatInfoTables.at( m_renderFormat ).bpp;
}

void CImage::SetRGB(int32_t x, int32_t y, unsigned char r, unsigned char g, unsigned char b)
{
    int32_t nStride = HasAlpha() ? 4 : 3;
    int32_t nIndex = (y * m_width + x) * nStride;
    BEATS_ASSERT(nIndex < m_dataLen);
    m_data[nIndex++] = r;
    BEATS_ASSERT(nIndex < m_dataLen);
    m_data[nIndex++] = g;
    BEATS_ASSERT(nIndex < m_dataLen);
    m_data[nIndex++] = b;
}

unsigned char CImage::GetAlpha(int32_t x, int32_t y) const
{
    unsigned char ret = 0;
    if (HasAlpha())
    {
        int32_t nIndex = (y * m_width + x) * 4;
        BEATS_ASSERT(nIndex + 3 < m_dataLen);
        ret = m_data[nIndex + 3];
    }
    return ret;
}

bool CImage::HasAlpha() const
{
    return _pixelFormatInfoTables.at( m_renderFormat ).alpha;
}

bool CImage::IsCompressed() const
{
    return _pixelFormatInfoTables.at( m_renderFormat ).compressed;
}

int CImage::CcNextPOT( int x )
{
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >>16);
    return x + 1;
}

const PixelFormatInfoMap& CImage::GetPixelFormatInfoMap()
{
    return _pixelFormatInfoTables;
}

bool CImage::InitWithImageData(const unsigned char* data, ssize_t dataLen, const TCHAR* filename)
{
    bool ret = false;

    if (data && dataLen > 0)
    {
        unsigned char* unpackedData = NULL;
        ssize_t unpackedLen = 0;

        //detect and unzip the compress file, not support for now.
        //if (CZipUtils::IsCCZBuffer(data, dataLen))
        //{
        //    unpackedLen = CZipUtils::InflateCCZBuffer(data, dataLen, &unpackedData);
        //}
        //else if (CZipUtils::IsGZipBuffer(data, dataLen))
        //{
        //    unpackedLen = CZipUtils::InflateMemory(const_cast<unsigned char*>(data), dataLen, &unpackedData);
        //}
        //else
        //{
            unpackedData = const_cast<unsigned char*>(data);
            unpackedLen = dataLen;
        //}

        m_fileType = DetectFormat(unpackedData, unpackedLen);

        switch (m_fileType)
        {
        case EFormat::eF_PNG:
            ret = InitWithPngData(unpackedData, unpackedLen, filename);
            break;
        case EFormat::eF_JPG:
            ret = InitWithJpgData(unpackedData, unpackedLen, filename);
            break;
        case EFormat::eF_TIFF:
            ret = InitWithTiffData(unpackedData, unpackedLen, filename);
            break;
        case EFormat::eF_WEBP:
            ret = InitWithWebpData(unpackedData, unpackedLen, filename);
            break;
        case EFormat::eF_PVR:
            ret = InitWithPVRData(unpackedData, unpackedLen, filename);
            break;
        case EFormat::eF_ETC:
            ret = InitWithETCData(unpackedData, unpackedLen, filename);
            break;
        case EFormat::eF_S3TC:
            ret = InitWithS3TCData(unpackedData, unpackedLen, filename);
            break;
        case EFormat::eF_ATITC:
            ret = InitWithATITCData(unpackedData, unpackedLen, filename);
            break;
        default:
            {
                // load and detect image format
                TImageTGA* tgaData = CTgaLib::TgaLoadBuffer(unpackedData, unpackedLen);

                if (tgaData != NULL && tgaData->status == TGA_OK)
                {
                    ret = InitWithTGAData(tgaData, filename);
                }
                else
                {
                    BEATS_ASSERT(false, _T("unsupport image format!"));
                }

                BEATS_SAFE_DELETE(tgaData);
                break;
            }
        }
        if (unpackedData != data)
        {
            BEATS_SAFE_DELETE_ARRAY(unpackedData);
        }
    }
    return ret;
}

bool CImage::InitWithJpgData(const unsigned char * data, ssize_t dataLen, const TCHAR* filename)
{
    /* these are standard libjpeg structures for reading(decompression) */
    jpeg_decompress_struct cinfo;
    /* We use our private extension JPEG error handler.
     * Note that this struct must live as long as the main JPEG parameter
     * struct, to avoid dangling-pointer problems.
     */
    SErrorMgr jerr;
    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1] = {0};
    bool bRet = false;
    do 
    {
        /* We set up the normal JPEG error routines, then override error_exit. */
        cinfo.err = jpeg_std_error(&jerr.pub);
        jerr.pub.error_exit = ErrorExit;
        /* Establish the setjmp return context for MyErrorExit to use. */
        if (setjmp(jerr.setjmp_buffer)) {
            /* If we get here, the JPEG code has signaled an error.
             * We need to clean up the JPEG object, close the input file, and return.
             */
            jpeg_destroy_decompress(&cinfo);
            break;
        }

        /* setup decompression process and source, then read JPEG header */
        jpeg_create_decompress( &cinfo );

#ifndef CC_TARGET_QT5
        jpeg_mem_src( &cinfo, const_cast<unsigned char*>(data), dataLen );
#endif /* CC_TARGET_QT5 */

        /* reading the image header which contains image information */
#if (JPEG_LIB_VERSION >= 90)
        // libjpeg 0.9 adds stricter types.
        jpeg_read_header( &cinfo, TRUE );
#else
        jpeg_read_header( &cinfo, true );
#endif

        // we only support RGB or grayscale
        if (cinfo.jpeg_color_space == JCS_GRAYSCALE)
        {
            m_renderFormat = PixelFormat::I8;
        }else
        {
            cinfo.out_color_space = JCS_RGB;
            m_renderFormat = PixelFormat::RGB888;
        }

        /* Start decompression jpeg here */
        jpeg_start_decompress( &cinfo );

        /* init image info */
        m_width  = cinfo.output_width;
        m_height = cinfo.output_height;
        m_preMulti = false;
        row_pointer[0] = BEATS_NEW_ARRAY(unsigned char, cinfo.output_width*cinfo.output_components, "Texture", filename);
        if(! row_pointer[0])
        {
            break;
        }
        m_dataLen = cinfo.output_width*cinfo.output_height*cinfo.output_components;
        m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);
        if(! m_data)
        {
            break;
        }

        /* now actually read the jpeg into the raw buffer */
        /* read one scan line at a time */
        while( cinfo.output_scanline < cinfo.output_height )
        {
            jpeg_read_scanlines( &cinfo, row_pointer, 1 );
            uint32_t location = (cinfo.output_height - cinfo.output_scanline - 1) * cinfo.output_width * cinfo.output_components;
            for(uint32_t i = 0; i < cinfo.output_width;i++) 
            {
                for (int j = 0; j < cinfo.output_components; ++j)
                {
                    m_data[location++] = row_pointer[0][i * cinfo.output_components + j];
                }
            }
        }

        /* When read image file with broken data, jpeg_finish_decompress() may cause error.
         * Besides, jpeg_destroy_decompress() shall deallocate and release all memory associated
         * with the decompression object.
         * So it doesn't need to call jpeg_finish_decompress().
         */
        //jpeg_finish_decompress( &cinfo );
        jpeg_destroy_decompress( &cinfo );
        /* wrap up decompression, destroy objects, free pointers and close open files */        
        bRet = true;
    } while (0);

    if (row_pointer[0] != nullptr)
    {
        BEATS_SAFE_DELETE_ARRAY(row_pointer[0]);
    };
    return bRet;
}

bool CImage::InitWithPngData(const unsigned char * data, ssize_t dataLen, const TCHAR* filename)
{
#define PNGSIGSIZE  8
    bool bRet = false;
    png_byte        header[PNGSIGSIZE]   = {0}; 
    png_structp     png_ptr     =   0;
    png_infop       info_ptr    = 0;

    do 
    {
        // png header len is 8 bytes
        if(dataLen < PNGSIGSIZE)
        {
            break;
        }
        // check the data is png or not
        memcpy(header, data, PNGSIGSIZE);
        if(png_sig_cmp(header, 0, PNGSIGSIZE))
        {
            break;
        }
        // init png_struct
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        if(! png_ptr)
        {
            break;
        }
        // init png_info
        info_ptr = png_create_info_struct(png_ptr);
        if(!info_ptr)
        {
            break;
        }

#if (BEYONDENGINE_PLATFORM != PLATFORM_BADA && BEYONDENGINE_PLATFORM != PLATFORM_NACL)
        if(setjmp(png_jmpbuf(png_ptr)))
        {
            break;
        }
#endif

        // set the read call back function
        TImageSource imageSource;
        imageSource.data    = (unsigned char*)data;
        imageSource.size    = dataLen;
        imageSource.offset  = 0;
        png_set_read_fn(png_ptr, &imageSource, PngReadCallback);

        // read png header info

        // read png file info
        png_read_info(png_ptr, info_ptr);

        m_width = png_get_image_width(png_ptr, info_ptr);
        m_height = png_get_image_height(png_ptr, info_ptr);
        png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);

        // force palette images to be expanded to 24-bit RGB
        // it may include alpha channel
        if (color_type == PNG_COLOR_TYPE_PALETTE)
        {
            png_set_palette_to_rgb(png_ptr);
        }
        // low-bit-depth grayscale images are to be expanded to 8 bits
        if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        {
            bit_depth = 8;
            png_set_expand_gray_1_2_4_to_8(png_ptr);
        }
        // expand any tRNS chunk data into a full alpha channel
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        {
            png_set_tRNS_to_alpha(png_ptr);
        }  
        // reduce images with 16-bit samples to 8 bits
        if (bit_depth == 16)
        {
            png_set_strip_16(png_ptr);
        } 

        // Expanded earlier for grayscale, now take care of palette and rgb
        if (bit_depth < 8)
        {
            png_set_packing(png_ptr);
        }
        // update info
        png_read_update_info(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        color_type = png_get_color_type(png_ptr, info_ptr);

        switch (color_type)
        {
        case PNG_COLOR_TYPE_GRAY:
            m_renderFormat = PixelFormat::I8;
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            m_renderFormat = PixelFormat::AI88;
            break;
        case PNG_COLOR_TYPE_RGB:
            m_renderFormat = PixelFormat::RGB888;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            m_renderFormat = PixelFormat::RGBA8888;
            break;
        default:
            break;
        }

        // read png data
        png_size_t rowbytes;
        png_bytep* row_pointers = BEATS_NEW_ARRAY(png_bytep, m_height, "Texture", filename);
        rowbytes = png_get_rowbytes(png_ptr, info_ptr);

        m_dataLen = rowbytes * m_height;
        m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);
        BEATS_ASSERT(m_data != NULL);
        if(!m_data)
        {
            break;
        }
        for (unsigned short i = 0; i < m_height; ++i)
        {
            row_pointers[i] = m_data + (m_height - i - 1) * rowbytes;
        }
        png_read_image(png_ptr, row_pointers);

        png_read_end(png_ptr, NULL);

        m_preMulti = false;

        if (row_pointers != NULL)
        {
            BEATS_SAFE_DELETE_ARRAY(row_pointers);
        };
        bRet = true;
    } while (0);

    if (png_ptr)
    {
        png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
    }
    return bRet;
}

bool CImage::InitWithTiffData(const unsigned char * data, ssize_t dataLen, const TCHAR* filename)
{
    bool bRet = false;
    do 
    {
        // set the read call back function
        TImageSource imageSource;
        imageSource.data    = data;
        imageSource.size    = dataLen;
        imageSource.offset  = 0;

        TIFF* tif = TIFFClientOpen("file.tif", "r", (thandle_t)&imageSource, 
            tiffReadProc, tiffWriteProc,
            tiffSeekProc, tiffCloseProc, tiffSizeProc,
            tiffMapProc,
            tiffUnmapProc);

        if( NULL == tif )
        {
            break;
        }
        uint32 w = 0, h = 0;
        uint16 bitsPerSample = 0, samplePerPixel = 0, planarConfig = 0;
        uint32_t npixels = 0;
        
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
        TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
        TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);
        TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planarConfig);

        npixels = w * h;
        
        m_renderFormat = PixelFormat::RGBA8888;
        m_width = w;
        m_height = h;

        m_dataLen = npixels * sizeof (uint32);
        m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);
        uint32* raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
        if (raster != nullptr) 
        {
           if (TIFFReadRGBAImageOriented(tif, w, h, raster, ORIENTATION_BOTLEFT, 0))
           {
                /* the raster data is pre-multiplied by the alpha component 
                   after invoking TIFFReadRGBAImageOriented*/
                m_preMulti = true;

               memcpy(m_data, raster, npixels*sizeof (uint32));
           }

          _TIFFfree(raster);
        }
        

        TIFFClose(tif);

        bRet = true;
    } while (0);
    return bRet;
}

bool CImage::InitWithWebpData(const unsigned char * data, ssize_t dataLen, const TCHAR* filename)
{
    bool bRet = false;  

#if (BEYONDENGINE_PLATFORM == PLATFORM_WP8) || (BEYONDENGINE_PLATFORM == PLATFORM_WINRT)
    BEATS_PRINT(_T("WEBP image format not supported on WinRT or WP8"));
#else
    do
    {
        WebPDecoderConfig config;
        if (WebPInitDecoderConfig(&config) == 0) break;
        if (WebPGetFeatures(static_cast<const uint8_t*>(data), dataLen, &config.input) != VP8_STATUS_OK) break;
        if (config.input.width == 0 || config.input.height == 0) break;

        config.output.colorspace = MODE_RGBA;
        m_renderFormat = PixelFormat::RGBA8888;
        m_width    = config.input.width;
        m_height   = config.input.height;

        m_dataLen = m_width * m_height * 4;
        m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);

        config.output.u.RGBA.rgba = static_cast<uint8_t*>(m_data);
        config.output.u.RGBA.stride = m_width * 4;
        config.output.u.RGBA.size = m_dataLen;
        config.output.is_external_memory = 1;

        if (WebPDecode(static_cast<const uint8_t*>(data), dataLen, &config) != VP8_STATUS_OK)
        {
            BEATS_SAFE_DELETE_ARRAY( m_data );
            break;
        }

        bRet = true;
    } while (0);
#endif
    return bRet;
}

bool CImage::InitWithPVRData(const unsigned char * data, ssize_t dataLen, const TCHAR* filename)
{
    return InitWithPVRv2Data(data, dataLen, filename) || InitWithPVRv3Data(data, dataLen, filename);
}

bool CImage::InitWithPVRv2Data(const unsigned char * data, ssize_t dataLen, const TCHAR* filename)
{
    int dataLength = 0, dataOffset = 0, dataSize = 0;
    int blockSize = 0, widthBlocks = 0, heightBlocks = 0;
    int width = 0, height = 0;
    bool bReturn = false;
    do 
    {
        //Cast first sizeof(PVRTexHeader) bytes of data stream as PVRTexHeader
        const PVRv2TexHeader *header = static_cast<const PVRv2TexHeader *>(static_cast<const void*>(data));

        //Make sure that tag is in correct formatting
        if (memcmp(&header->pvrTag, gPVRTexIdentifier, strlen(gPVRTexIdentifier)) != 0)
        {
            break;
        }

        CConfiguration *configuration = CConfiguration::GetInstance();

        m_hasPremultipliedAlpha = false;
        unsigned int flags = BEYONDENGINE_SWAP_INT32_LITTLE_TO_HOST(header->flags);
        PVR2TexturePixelFormat formatFlags = static_cast<PVR2TexturePixelFormat>(flags & PVR_TEXTURE_FLAG_TYPE_MASK);
        bool flipped = (flags & (unsigned int)PVR2TextureFlag::VerticalFlip) ? true : false;
        if (flipped)
        {
            BEATS_PRINT(_T("WARNING: Image is flipped. Regenerate it using PVRTexTool"));
        }

        if (! configuration->SupportsNPOT() &&
            (static_cast<int>(header->width) != CcNextPOT(header->width)
            || static_cast<int>(header->height) != CcNextPOT(header->height)))
        {
            BEATS_PRINT(_T("ERROR: Loading an NPOT texture (%dx%d) but is not supported on this device"), header->width, header->height);
            break;
        }

        if (!testFormatForPvr2TCSupport(formatFlags))
        {
            BEATS_PRINT(_T("WARNING: Unsupported PVR Pixel Format: 0x%02X. Re-encode it with a OpenGL pixel format variant"), (int)formatFlags);
            break;
        }

        if (v2_pixel_formathash.find(formatFlags) == v2_pixel_formathash.end())
        {
            BEATS_PRINT(_T("WARNING: Unsupported PVR Pixel Format: 0x%02X. Re-encode it with a OpenGL pixel format variant"), (int)formatFlags);
            break;
        }

        auto it = _pixelFormatInfoTables.find(v2_pixel_formathash.at(formatFlags));

        if (it == _pixelFormatInfoTables.end())
        {
            BEATS_PRINT(_T("WARNING: Unsupported PVR Pixel Format: 0x%02X. Re-encode it with a OpenGL pixel format variant"), (int)formatFlags);
            break;
        }

        m_renderFormat = it->first;

        //Reset num of mipmaps
        m_numberOfMipmaps = 0;

        //Get size of mipmap
        m_width = width = BEYONDENGINE_SWAP_INT32_LITTLE_TO_HOST(header->width);
        m_height = height = BEYONDENGINE_SWAP_INT32_LITTLE_TO_HOST(header->height);

        //Get ptr to where data starts..
        dataLength = BEYONDENGINE_SWAP_INT32_LITTLE_TO_HOST(header->dataLength);

        //Move by size of header
        m_dataLen = dataLen - sizeof(PVRv2TexHeader);
        m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);
        memcpy(m_data, (unsigned char*)data + sizeof(PVRv2TexHeader), m_dataLen);

        bool bBreake = false;
        // Calculate the data size for each texture level and respect the minimum number of blocks
        while (dataOffset < dataLength)
        {
            switch (formatFlags) {
            case PVR2TexturePixelFormat::PVRTC2BPP_RGBA:
                blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
                widthBlocks = width / 8;
                heightBlocks = height / 4;
                break;
            case PVR2TexturePixelFormat::PVRTC4BPP_RGBA:
                blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
                widthBlocks = width / 4;
                heightBlocks = height / 4;
                break;
            case PVR2TexturePixelFormat::BGRA8888:
                if (CConfiguration::GetInstance()->SupportsBGRA8888() == false)
                {
                    BEATS_PRINT(_T("Image. BGRA8888 not supported on this device"));
                    bBreake = true;
                    break;
                }
            default:
                blockSize = 1;
                widthBlocks = width;
                heightBlocks = height;
                break;
            }

            // Clamp to minimum number of blocks
            if (widthBlocks < 2)
            {
                widthBlocks = 2;
            }
            if (heightBlocks < 2)
            {
                heightBlocks = 2;
            }

            dataSize = widthBlocks * heightBlocks * ((blockSize  * it->second.bpp) / 8);
            int packetLength = (dataLength - dataOffset);
            packetLength = packetLength > dataSize ? dataSize : packetLength;

            //Make record to the mipmaps array and increment counter
            m_mipmaps[m_numberOfMipmaps].address = m_data + dataOffset;
            m_mipmaps[m_numberOfMipmaps].len = packetLength;
            m_numberOfMipmaps++;

            dataOffset += packetLength;

            //Update width and height to the next lower power of two
            width = MAX(width >> 1, 1);
            height = MAX(height >> 1, 1);
        }
        if ( bBreake )
        {
            break;
        }
        
        bReturn = true;
    } while (0);
    return bReturn;
}

bool CImage::InitWithPVRv3Data(const unsigned char * data, ssize_t dataLen, const TCHAR* filename)
{
    bool bRet = false;
    if (static_cast<size_t>(dataLen) >= sizeof(PVRv3TexHeader))
    {
        const PVRv3TexHeader *header = static_cast<const PVRv3TexHeader *>(static_cast<const void*>(data));
        // validate version
        BEATS_ASSERT(BEYONDENGINE_SWAP_INT32_BIG_TO_HOST(header->version) == 0x50565203, "WARNING: pvr file version mismatch");
        if (BEYONDENGINE_SWAP_INT32_BIG_TO_HOST(header->version) == 0x50565203)
        {
            // parse pixel format
            PVR3TexturePixelFormat pixelFormat = static_cast<PVR3TexturePixelFormat>(header->pixelFormat);
            bool bSupportFormat = testFormatForPvr3TCSupport(pixelFormat) &&
                    v3_pixel_formathash.find(pixelFormat) != v3_pixel_formathash.end() &&
                    _pixelFormatInfoTables.find(getDevicePixelFormat(v3_pixel_formathash.at(pixelFormat))) != _pixelFormatInfoTables.end();
            BEATS_ASSERT(bSupportFormat, "WARNING: Unsupported PVR Pixel Format: 0x%016llX. Re-encode it with a OpenGL pixel format variant", static_cast<unsigned long long>(pixelFormat));
            if (bSupportFormat)
            {
                auto it = _pixelFormatInfoTables.find(getDevicePixelFormat(v3_pixel_formathash.at(pixelFormat)));
                m_renderFormat = it->first;
                int bpp = it->second.bpp;

                // flags
                int flags = BEYONDENGINE_SWAP_INT32_LITTLE_TO_HOST(header->flags);

                // PVRv3 specifies premultiply alpha in a flag -- should always respect this in PVRv3 files
                m_hasPremultipliedAlpha = (flags & (unsigned int)PVR3TextureFlag::PremultipliedAlpha) != 0;

                // sizing
                int width = BEYONDENGINE_SWAP_INT32_LITTLE_TO_HOST(header->width);
                int height = BEYONDENGINE_SWAP_INT32_LITTLE_TO_HOST(header->height);
                m_width = width;
                m_height = height;
                int dataOffset = 0, dataSize = 0;
                int blockSize = 0, widthBlocks = 0, heightBlocks = 0;

                m_dataLen = dataLen - (sizeof(PVRv3TexHeader)+header->metadataLength);
                m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);
                memcpy(m_data, static_cast<const unsigned char*>(data)+sizeof(PVRv3TexHeader)+header->metadataLength, m_dataLen);

                m_numberOfMipmaps = header->numberOfMipmaps;
                BEATS_ASSERT(m_numberOfMipmaps < MIPMAP_MAX, "Image: Maximum number of mimpaps reached. Increase the CC_MIPMAP_MAX value");

                for (int i = 0; i < m_numberOfMipmaps; i++)
                {
                    switch ((PVR3TexturePixelFormat)pixelFormat)
                    {
                    case PVR3TexturePixelFormat::PVRTC2BPP_RGB:
                    case PVR3TexturePixelFormat::PVRTC2BPP_RGBA:
                        if (!CConfiguration::GetInstance()->SupportsPVRTC())
                        {
                            BEATS_PRINT("Hardware PVR decoder not present. Using software decoder");
                            m_bUnpack = true;
                            m_mipmaps[i].len = width*height * 4;
                            m_mipmaps[i].address = BEATS_NEW_ARRAY(unsigned char, width*height * 4, "Texture", filename);
                            PVRTDecompressPVRTC(m_data + dataOffset, width, height, m_mipmaps[i].address, true);
                            bpp = 2;
                        }
                        blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
                        widthBlocks = width / 8;
                        heightBlocks = height / 4;
                        break;
                    case PVR3TexturePixelFormat::PVRTC4BPP_RGB:
                    case PVR3TexturePixelFormat::PVRTC4BPP_RGBA:
                        if (!CConfiguration::GetInstance()->SupportsPVRTC())
                        {
                            BEATS_PRINT("Hardware PVR decoder not present. Using software decoder");
                            m_bUnpack = true;
                            m_mipmaps[i].len = width*height * 4;
                            m_mipmaps[i].address = BEATS_NEW_ARRAY(unsigned char, width*height * 4, "Texture", filename);
                            PVRTDecompressPVRTC(m_data + dataOffset, width, height, m_mipmaps[i].address, false);
                            bpp = 4;
                        }
                        blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
                        widthBlocks = width / 4;
                        heightBlocks = height / 4;
                        break;
                    case PVR3TexturePixelFormat::ETC1:
                        if (!CConfiguration::GetInstance()->SupportsETC())
                        {
                            BEATS_PRINT("Hardware ETC1 decoder not present. Using software decoder");
                            int bytePerPixel = 3;
                            unsigned int stride = width * bytePerPixel;
                            m_bUnpack = true;
                            m_mipmaps[i].len = width*height*bytePerPixel;
                            m_mipmaps[i].address = BEATS_NEW_ARRAY(unsigned char, width*height * bytePerPixel, "Texture", filename);
                            if (etc1_decode_image(static_cast<const unsigned char*>(m_data + dataOffset), static_cast<etc1_byte*>(m_mipmaps[i].address), width, height, bytePerPixel, stride) != 0)
                            {
                                return false;
                            }
                        }
                        blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
                        widthBlocks = width / 4;
                        heightBlocks = height / 4;
                        break;
                    case PVR3TexturePixelFormat::BGRA8888:
                        if (!CConfiguration::GetInstance()->SupportsBGRA8888())
                        {
                            BEATS_PRINT("Image. BGRA8888 not supported on this device");
                            return false;
                        }
                    default:
                        blockSize = 1;
                        widthBlocks = width;
                        heightBlocks = height;
                        break;
                    }

                    // Clamp to minimum number of blocks
                    if (widthBlocks < 2)
                    {
                        widthBlocks = 2;
                    }
                    if (heightBlocks < 2)
                    {
                        heightBlocks = 2;
                    }

                    dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
                    auto packetLength = m_dataLen - dataOffset;
                    packetLength = packetLength > dataSize ? dataSize : packetLength;

                    if (!m_bUnpack)
                    {
                        m_mipmaps[i].address = m_data + dataOffset;
                        m_mipmaps[i].len = static_cast<int>(packetLength);
                    }

                    dataOffset += packetLength;
                    BEATS_ASSERT(dataOffset <= m_dataLen, "CCTexurePVR: Invalid length");


                    width = MAX(width >> 1, 1);
                    height = MAX(height >> 1, 1);
                }

                if (m_bUnpack)
                {
                    m_data = m_mipmaps[0].address;
                    m_dataLen = m_mipmaps[0].len;
                }
                bRet = true;
            }
        }
    }
    return bRet;
}

bool CImage::InitWithETCData(const unsigned char * data, ssize_t dataLen, const TCHAR* filename)
{
    bool bRet = false;
    const etc1_byte* header = static_cast<const etc1_byte*>(data);
    BEYONDENGINE_UNUSED_PARAM(dataLen);
    //check the data
    if (etc1_pkm_is_valid(header))
    {
        m_width = etc1_pkm_get_width(header);
        m_height = etc1_pkm_get_height(header);
        BEATS_ASSERT(0 != m_width && 0 != m_height);
        if (CConfiguration::GetInstance()->SupportsETC())
        {
            //old opengl version has no define for GL_ETC1_RGB8_OES, add macro to make compiler happy.
#ifdef GL_ETC1_RGB8_OES
            m_renderFormat = PixelFormat::ETC;
            m_dataLen = etc1_get_encoded_data_size(m_width, m_height);
            BEATS_ASSERT(m_dataLen <= dataLen);
            m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);
            memcpy(m_data, static_cast<const unsigned char*>(data)+ETC_PKM_HEADER_SIZE, m_dataLen);
            bRet = true;
#endif
        }
        else
        {
            //if it is not gles or device do not support ETC, decode texture by software
            int bytePerPixel = 3;
            unsigned int stride = m_width * bytePerPixel;
            m_renderFormat = PixelFormat::RGB888;

            m_dataLen = m_width * m_height * bytePerPixel;
            m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);
            bRet = etc1_decode_image(static_cast<const unsigned char*>(data)+ETC_PKM_HEADER_SIZE, static_cast<etc1_byte*>(m_data), m_width, m_height, bytePerPixel, stride) == 0;
            if (!bRet)
            {
                m_dataLen = 0;
                BEATS_SAFE_DELETE_ARRAY(m_data);
            }
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
            for (int32_t i = 0; i < m_height / 2; ++i)
            {
                for (int32_t j = 0; j < m_width; ++j)
                {
                    uint32_t uCurIndex = (j + i * m_width) * bytePerPixel;
                    uint32_t uMirrorIndex = (j + (m_height - 1 - i) * m_width) * bytePerPixel;
                    for (int k = 0; k < 3; ++k)
                    {
                        unsigned char Tmp = m_data[uCurIndex];
                        m_data[uCurIndex] = m_data[uMirrorIndex];
                        m_data[uMirrorIndex] = Tmp;
                        ++uCurIndex;
                        ++uMirrorIndex;
                    }
                }
            }
#endif
        }
    }
    return bRet;
}

bool CImage::InitWithS3TCData(const unsigned char * data, ssize_t dataLen, const TCHAR* filename)
{
    const uint32_t FOURCC_DXT1 = makeFourCC('D', 'X', 'T', '1');
    const uint32_t FOURCC_DXT3 = makeFourCC('D', 'X', 'T', '3');
    const uint32_t FOURCC_DXT5 = makeFourCC('D', 'X', 'T', '5');

    /* load the .dds file */

    S3TCTexHeader *header = (S3TCTexHeader *)data;
    unsigned char *pixelData = BEATS_NEW_ARRAY(unsigned char, dataLen - sizeof(S3TCTexHeader), "Texture", filename);
    memcpy((void *)pixelData, data + sizeof(S3TCTexHeader), dataLen - sizeof(S3TCTexHeader));

    m_width = header->ddsd.width;
    m_height = header->ddsd.height;
    m_numberOfMipmaps = MAX(1, header->ddsd.DUMMYUNIONNAMEN2.mipMapCount); //if dds header reports 0 mipmaps, set to 1 to force correct software decoding (if needed).
    m_dataLen = 0;
    int blockSize = (FOURCC_DXT1 == header->ddsd.DUMMYUNIONNAMEN4.ddpfPixelFormat.fourCC) ? 8 : 16;

    /* calculate the dataLen */

    int width = m_width;
    int height = m_height;

    if (CConfiguration::GetInstance()->SupportsS3TC())  //compressed data length
    {
        m_dataLen = dataLen - sizeof(S3TCTexHeader);
        m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);
        memcpy((void *)m_data,(void *)pixelData , m_dataLen);
    }
    else                                               //decompressed data length
    {
        for (int i = 0; i < m_numberOfMipmaps && (width || height); ++i)
        {
            if (width == 0) width = 1;
            if (height == 0) height = 1;

            m_dataLen += (height * width *4);

            width >>= 1;
            height >>= 1;
        }
        m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);
    }

    /* if hardware supports s3tc, set pixel format before loading mipmaps, to support non-mipmapped textures  */
    if (CConfiguration::GetInstance()->SupportsS3TC())
    {   //decode texture through hardware

        if (FOURCC_DXT1 == header->ddsd.DUMMYUNIONNAMEN4.ddpfPixelFormat.fourCC)
        {
            m_renderFormat = PixelFormat::S3TC_DXT1;
        }
        else if (FOURCC_DXT3 == header->ddsd.DUMMYUNIONNAMEN4.ddpfPixelFormat.fourCC)
        {
            m_renderFormat = PixelFormat::S3TC_DXT3;
        }
        else if (FOURCC_DXT5 == header->ddsd.DUMMYUNIONNAMEN4.ddpfPixelFormat.fourCC)
        {
            m_renderFormat = PixelFormat::S3TC_DXT5;
        }
    } else { //will software decode
        m_renderFormat = PixelFormat::RGBA8888;
    }

    /* load the mipmaps */

    int encodeOffset = 0;
    int decodeOffset = 0;
    width = m_width;  height = m_height;

    for (int i = 0; i < m_numberOfMipmaps && (width || height); ++i)  
    {
        if (width == 0) width = 1;
        if (height == 0) height = 1;

        int size = ((width+3)/4)*((height+3)/4)*blockSize;

        if (CConfiguration::GetInstance()->SupportsS3TC())
        {   //decode texture through hardware
            m_mipmaps[i].address = (unsigned char *)m_data + encodeOffset;
            m_mipmaps[i].len = size;
        }
        else
        {   //if it is not gles or device do not support S3TC, decode texture by software

            BEATS_PRINT(_T("Hardware S3TC decoder not present. Using software decoder"));

            int bytePerPixel = 4;
            unsigned int stride = width * bytePerPixel;

            std::vector<unsigned char> decodeImageData(stride * height);
            if (FOURCC_DXT1 == header->ddsd.DUMMYUNIONNAMEN4.ddpfPixelFormat.fourCC)
            {
                CS3tc::S3tc_decode(pixelData + encodeOffset, &decodeImageData[0], width, height, ES3TCDecodeFlag::DXT1);
            }
            else if (FOURCC_DXT3 == header->ddsd.DUMMYUNIONNAMEN4.ddpfPixelFormat.fourCC)
            {
                CS3tc::S3tc_decode(pixelData + encodeOffset, &decodeImageData[0], width, height, ES3TCDecodeFlag::DXT3);
            }
            else if (FOURCC_DXT5 == header->ddsd.DUMMYUNIONNAMEN4.ddpfPixelFormat.fourCC)
            {
                CS3tc::S3tc_decode(pixelData + encodeOffset, &decodeImageData[0], width, height, ES3TCDecodeFlag::DXT5);
            }

            m_mipmaps[i].address = (unsigned char *)m_data + decodeOffset;
            m_mipmaps[i].len = (stride * height);
            memcpy((void *)m_mipmaps[i].address, (void *)&decodeImageData[0], m_mipmaps[i].len);
            decodeOffset += stride * height;
        }

        encodeOffset += size;
        width >>= 1;
        height >>= 1;
    }

    /* end load the mipmaps */

    if (pixelData != NULL)
    {
        BEATS_SAFE_DELETE_ARRAY(pixelData);
    };

    return true;
}



bool CImage::InitWithATITCData(const unsigned char *data, ssize_t dataLen, const TCHAR* filename)
{
    /* load the .ktx file */
    ATITCTexHeader *header = (ATITCTexHeader *)data;
    m_width =  header->pixelWidth;
    m_height = header->pixelHeight;
    m_numberOfMipmaps = header->numberOfMipmapLevels;

    int blockSize = 0;
    switch (header->glInternalFormat)
    {
    case CC_GL_ATC_RGB_AMD:
        blockSize = 8;
        break;
    case CC_GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
        blockSize = 16;
        break;
    case CC_GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
        blockSize = 16;
        break;
    default:
        break;
    }

    /* pixelData point to the compressed data address */
    unsigned char *pixelData = (unsigned char *)data + sizeof(ATITCTexHeader) + header->bytesOfKeyValueData + 4;

    /* caculate the dataLen */
    int width = m_width;
    int height = m_height;

    if (CConfiguration::GetInstance()->SupportsATITC())  //compressed data length
    {
        m_dataLen = dataLen - sizeof(ATITCTexHeader) - header->bytesOfKeyValueData - 4;
        m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);
        memcpy((void *)m_data,(void *)pixelData , m_dataLen);
    }
    else                                               //decompressed data length
    {
        for (int i = 0; i < m_numberOfMipmaps && (width || height); ++i)
        {
            if (width == 0) width = 1;
            if (height == 0) height = 1;

            m_dataLen += (height * width *4);

            width >>= 1;
            height >>= 1;
        }
        m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);
    }

    /* load the mipmaps */
    int encodeOffset = 0;
    int decodeOffset = 0;
    width = m_width;  height = m_height;

    for (int i = 0; i < m_numberOfMipmaps && (width || height); ++i)
    {
        if (width == 0) width = 1;
        if (height == 0) height = 1;

        int size = ((width+3)/4)*((height+3)/4)*blockSize;

        if (CConfiguration::GetInstance()->SupportsATITC())
        {
            /* decode texture throught hardware */

            BEATS_PRINT(_T("this is atitc H decode"));

            switch (header->glInternalFormat)
            {
            case CC_GL_ATC_RGB_AMD:
                m_renderFormat = PixelFormat::ATC_RGB;
                break;
            case CC_GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
                m_renderFormat = PixelFormat::ATC_EXPLICIT_ALPHA;
                break;
            case CC_GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
                m_renderFormat = PixelFormat::ATC_INTERPOLATED_ALPHA;
                break;
            default:
                break;
            }

            m_mipmaps[i].address = (unsigned char *)m_data + encodeOffset;
            m_mipmaps[i].len = size;
        }
        else
        {
            /* if it is not gles or device do not support ATITC, decode texture by software */

            BEATS_PRINT(_T("Hardware ATITC decoder not present. Using software decoder"));

            int bytePerPixel = 4;
            unsigned int stride = width * bytePerPixel;
            m_renderFormat = PixelFormat::RGBA8888;

            std::vector<unsigned char> decodeImageData(stride * height);
            switch (header->glInternalFormat)
            {
            case CC_GL_ATC_RGB_AMD:
                CAtitc::Atitc_decode(pixelData + encodeOffset, &decodeImageData[0], width, height, EATITCDecodeFlag::ATC_RGB);
                break;
            case CC_GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
                CAtitc::Atitc_decode(pixelData + encodeOffset, &decodeImageData[0], width, height, EATITCDecodeFlag::ATC_EXPLICIT_ALPHA);
                break;
            case CC_GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
                CAtitc::Atitc_decode(pixelData + encodeOffset, &decodeImageData[0], width, height, EATITCDecodeFlag::ATC_INTERPOLATED_ALPHA);
                break;
            default:
                break;
            }

            m_mipmaps[i].address = (unsigned char *)m_data + decodeOffset;
            m_mipmaps[i].len = (stride * height);
            memcpy((void *)m_mipmaps[i].address, (void *)&decodeImageData[0], m_mipmaps[i].len);
            decodeOffset += stride * height;
        }

        encodeOffset += (size + 4);
        width >>= 1;
        height >>= 1;
    }
    /* end load the mipmaps */

    return true;
}

bool CImage::InitWithTGAData(TImageTGA* tgaData, const TCHAR* filename)
{
    bool ret = false;

    do
    {
        if(tgaData == NULL)
        {
            break;
        }

        // tgaLoadBuffer only support type 2, 3, 10
        if (2 == tgaData->type || 10 == tgaData->type)
        {
            // true color
            // unsupport RGB555
            if (tgaData->pixelDepth == 16)
            {
                m_renderFormat = PixelFormat::RGB5A1;
            }
            else if(tgaData->pixelDepth == 24)
            {
                m_renderFormat = PixelFormat::RGB888;
            }
            else if(tgaData->pixelDepth == 32)
            {
                m_renderFormat = PixelFormat::RGBA8888;
            }
            else
            {
                BEATS_PRINT(_T("Image WARNING: unsupport true color tga data pixel format. FILE: %s"), m_filePath.c_str());
                break;
            }
        }
        else if(3 == tgaData->type)
        {
            // gray
            if (8 == tgaData->pixelDepth)
            {
                m_renderFormat = PixelFormat::I8;
            }
            else
            {
                // actually this won't happen, if it happens, maybe the image file is not a tga
                BEATS_PRINT(_T("Image WARNING: unsupport gray tga data pixel format. FILE: %s"), m_filePath.c_str());
                break;
            }
        }

        m_width = tgaData->width;
        m_height = tgaData->height;
        m_dataLen = m_width * m_height * tgaData->pixelDepth / 8;
        m_fileType = EFormat::eF_TGA;
        m_preMulti = false;

    }while(false);

    m_data = BEATS_NEW_ARRAY(unsigned char, m_dataLen, "Texture", filename);
    if ( m_data )
    {
        ret = true;
        memcpy( m_data, tgaData->imageData, m_dataLen );
    }
    
    if (ret)
    {
        const unsigned char tgaSuffix[] = ".tga";
        for(int i = 0; i < 4; ++i)
        {
            if (tolower(m_filePath[m_filePath.length() - i - 1]) != tgaSuffix[3 - i])
            {
                BEATS_ASSERT( false,_T("Image WARNING: the image file suffix is not tga, but parsed as a tga image file. FILE: %s"), m_filePath.c_str());
                break;
            };
        }
    }
    else
    {
        if (tgaData->imageData != NULL)
        {
            BEATS_SAFE_DELETE_ARRAY( tgaData->imageData );
            m_data = NULL;
        }
    }

    return ret;
}

bool CImage::SaveImageToPNG( const TString& filePath)
{
    bool bRet = false;
    do 
    {
        FILE *fp;
        png_structp png_ptr;
        png_infop info_ptr;
        png_colorp palette;
        png_bytep *row_pointers;
        fp = fopen(filePath.c_str(), "wb");
        if(NULL == fp)
        {
            break;
        }

        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (nullptr == png_ptr)
        {
            fclose(fp);
            break;
        }

        info_ptr = png_create_info_struct(png_ptr);
        if (nullptr == info_ptr)
        {
            fclose(fp);
            png_destroy_write_struct(&png_ptr, nullptr);
            break;
        }
#if (BEYONDENGINE_PLATFORM != PLATFORM_BADA && BEYONDENGINE_PLATFORM != PLATFORM_NACL)
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            fclose(fp);
            png_destroy_write_struct(&png_ptr, &info_ptr);
            break;
        }
#endif
        png_init_io(png_ptr, fp);

        png_set_IHDR(png_ptr, info_ptr, m_width, m_height, 8, PNG_COLOR_TYPE_RGB,
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof (png_color));
        png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);
        png_write_info(png_ptr, info_ptr);
        png_set_packing(png_ptr);
        row_pointers = new png_bytep[ m_height ];
        if(row_pointers == nullptr)
        {
            fclose(fp);
            png_destroy_write_struct(&png_ptr, &info_ptr);
            break;
        }
        if (HasAlpha())
        {
            unsigned char *pTempData = new unsigned char[ m_width * m_height * 3 ];
            if (NULL == pTempData)
            {
                fclose(fp);
                png_destroy_write_struct(&png_ptr, &info_ptr);
                break;
            }

            for (int i = 0; i < m_height; ++i)
            {
                for (int j = 0; j < m_width; ++j)
                {
                    pTempData[(i * m_width + j) * 3] = m_data[(i * m_width + j) * 4];
                    pTempData[(i * m_width + j) * 3 + 1] = m_data[(i * m_width + j) * 4 + 1];
                    pTempData[(i * m_width + j) * 3 + 2] = m_data[(i * m_width + j) * 4 + 2];
                }
            }

            for (int i = 0; i < (int)m_height; i++)
            {
                row_pointers[m_height - i - 1] = (png_bytep)pTempData + i * m_width * 3;
            }

            png_write_image(png_ptr, row_pointers);

            BEATS_SAFE_DELETE_ARRAY(row_pointers);
            row_pointers = nullptr;

            if (pTempData != nullptr)
            {
                BEATS_SAFE_DELETE_ARRAY(pTempData);
            }
        }
        else
        {
            uint32_t uColorComponentCount = HasAlpha() ? 4 : 3;
            for (int i = 0; i < (int)m_height; i++)
            {
                row_pointers[m_height - i - 1] = (png_bytep)m_data + i * m_width * uColorComponentCount;
            }

            png_write_image(png_ptr, row_pointers);
            BEATS_SAFE_DELETE_ARRAY(row_pointers);
            row_pointers = nullptr;
        }
        png_write_end(png_ptr, info_ptr);
        png_free(png_ptr, palette);
        palette = nullptr;
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);

        bRet = true;
    } while (0);
    return bRet;
}

bool CImage::SaveImageToJPG( const TString& filePath )
{
    bool bRet = false;
    do 
    {
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;
        FILE * outfile;                 /* target file */
        JSAMPROW row_pointer[1];        /* pointer to JSAMPLE row[s] */
        int     row_stride;          /* physical row width in image buffer */

        cinfo.err = jpeg_std_error(&jerr);
        /* Now we can initialize the JPEG compression object. */
        jpeg_create_compress(&cinfo);

        if ((outfile = fopen(filePath.c_str(), "wb")) == NULL)
        {
            break;
        }

        jpeg_stdio_dest(&cinfo, outfile);

        cinfo.image_width = m_width;    /* image width and height, in pixels */
        cinfo.image_height = m_height;
        cinfo.input_components = 3;       /* # of color components per pixel */
        cinfo.in_color_space = JCS_RGB;       /* colorspace of input image */

        jpeg_set_defaults(&cinfo);

        jpeg_start_compress(&cinfo, TRUE);

        row_stride = m_width * 3; /* JSAMPLEs per row in image_buffer */

        if (HasAlpha())
        {
            unsigned char *pTempData = new unsigned char[m_width * m_height * 3 ];
            if (nullptr == pTempData)
            {
                jpeg_finish_compress(&cinfo);
                jpeg_destroy_compress(&cinfo);
                fclose(outfile);
                break;
            }
            for (int i = 0; i < m_height; ++i)
            {
                for (int j = 0; j < m_width; ++j)

                {
                    pTempData[(i * m_width + j) * 3] = m_data[(i * m_width + j) * 4];
                    pTempData[(i * m_width + j) * 3 + 1] = m_data[(i * m_width + j) * 4 + 1];
                    pTempData[(i * m_width + j) * 3 + 2] = m_data[(i * m_width + j) * 4 + 2];
                }
            }

            while (cinfo.next_scanline < cinfo.image_height) 
            {
                row_pointer[0] = & pTempData[(cinfo.image_height - cinfo.next_scanline - 1) * row_stride];
                (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
            }
            BEATS_SAFE_DELETE(pTempData);
        } 
        else
        {
            while (cinfo.next_scanline < cinfo.image_height) 
            {
                row_pointer[0] = & m_data[(cinfo.image_height - cinfo.next_scanline - 1) * row_stride];
                (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
            }
        }

        jpeg_finish_compress(&cinfo);
        fclose(outfile);
        jpeg_destroy_compress(&cinfo);

        bRet = true;
    } while (0);
    return bRet;
}

bool CImage::SaveImageToBMP(const TString& filePath)
{
    CSerializer serializer;
    BITMAPINFOHEADER bmih;
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = m_width;
    bmih.biHeight = m_height;
    bmih.biPlanes = 1;
    bmih.biBitCount = 32;
    bmih.biCompression = BI_RGB;
    bmih.biSizeImage = m_width * m_height * 4  ;

    BITMAPFILEHEADER bmfh;
    int nBitsOffset = sizeof(BITMAPFILEHEADER) + bmih.biSize;
    uint32_t lImageSize = bmih.biSizeImage;
    uint32_t lFileSize = nBitsOffset + lImageSize;
    bmfh.bfType = 'B' + ('M' << 8);
    bmfh.bfOffBits = nBitsOffset;
    bmfh.bfSize = lFileSize;
    bmfh.bfReserved1 = bmfh.bfReserved2 = 0;

    // Write the bitmap file header
    serializer.Serialize((const char*)&bmfh, sizeof(BITMAPFILEHEADER));

    // And then the bitmap info header
    serializer.Serialize((const char*)&bmih, sizeof(BITMAPINFOHEADER));

    // Finally, write the image data itself
    //-- the data represents our drawing
    for (int i = 0; i < m_dataLen;)
    {
        std::swap(m_data[i], m_data[i + 2]);
        m_data[i + 3] = 0xFF;
        i += 4;
    }
    serializer.Serialize(m_data, m_dataLen);
    serializer.Deserialize(filePath.c_str(), "wb+");
    return true;
}

bool CImage::InitWithImageFileThreadSafe( const TString& fullpath )
{
    bool ret = false;
    m_filePath = fullpath;
    CSerializer serializer(m_filePath.c_str());
    if (serializer.GetWritePos() > 0)
    {
        ret = InitWithImageData(serializer.GetBuffer(), serializer.GetWritePos(), fullpath.c_str());
    }

    return ret;
}

CImage::EFormat CImage::DetectFormat( const unsigned char * data, ssize_t dataLen )
{
    EFormat returnFormat = EFormat::eF_UNKOWN;
    if (IsPng(data, dataLen))
    {
        returnFormat = EFormat::eF_PNG;
    }
    else if (IsJpg(data, dataLen))
    {
        returnFormat = EFormat::eF_JPG;
    }
    else if (IsTiff(data, dataLen))
    {
        returnFormat = EFormat::eF_TIFF;
    }
    else if (IsWebp(data, dataLen))
    {
        returnFormat = EFormat::eF_WEBP;
    }
    else if (IsPvr(data, dataLen))
    {
        returnFormat = EFormat::eF_PVR;
    }
    else if (IsEtc(data, dataLen))
    {
        returnFormat = EFormat::eF_ETC;
    }
    else if (IsS3TC(data, dataLen))
    {
        returnFormat = EFormat::eF_S3TC;
    }
    else if (IsATITC(data, dataLen))
    {
        returnFormat = EFormat::eF_ATITC;
    }
    else
    {
        returnFormat = EFormat::eF_UNKOWN;
    }
    return returnFormat;
}

bool CImage::IsPng( const unsigned char * data, ssize_t dataLen )
{
    bool bReturn = false;
    if (dataLen > 8)
    {
        static const unsigned char PNG_SIGNATURE[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};

        bReturn = memcmp(PNG_SIGNATURE, data, sizeof(PNG_SIGNATURE)) == 0;
    }
    return bReturn;
}

bool CImage::IsJpg( const unsigned char * data, ssize_t dataLen )
{
    bool ret = false;
    if (dataLen > 4)
    {
        static const unsigned char JPG_SOI[] = {0xFF, 0xD8};

        ret = memcmp(data, JPG_SOI, 2) == 0;
    }
    return ret;
}

bool CImage::IsTiff( const unsigned char * data, ssize_t dataLen )
{
    bool ret = false;
    if (dataLen > 4)
    {
        static const char* TIFF_II = "II";
        static const char* TIFF_MM = "MM";

        ret = (memcmp(data, TIFF_II, 2) == 0 && *(static_cast<const unsigned char*>(data) + 2) == 42 && *(static_cast<const unsigned char*>(data) + 3) == 0) ||
            (memcmp(data, TIFF_MM, 2) == 0 && *(static_cast<const unsigned char*>(data) + 2) == 0 && *(static_cast<const unsigned char*>(data) + 3) == 42);
    }
    return ret;
}

bool CImage::IsWebp( const unsigned char * data, ssize_t dataLen )
{
    bool ret = false;
    if (dataLen > 12)
    {
        static const char* WEBP_RIFF = "RIFF";
        static const char* WEBP_WEBP = "WEBP";

        ret = memcmp(data, WEBP_RIFF, 4) == 0 
            && memcmp(static_cast<const unsigned char*>(data) + 8, WEBP_WEBP, 4) == 0;
    }

    return ret;
}

bool CImage::IsPvr( const unsigned char * data, ssize_t dataLen )
{
    bool ret = false;
    if (static_cast<uint32_t>(dataLen) > sizeof(PVRv2TexHeader) && static_cast<uint32_t>(dataLen) > sizeof(PVRv3TexHeader))
    {
        const PVRv2TexHeader* headerv2 = static_cast<const PVRv2TexHeader*>(static_cast<const void*>(data));
        const PVRv3TexHeader* headerv3 = static_cast<const PVRv3TexHeader*>(static_cast<const void*>(data));

        ret = memcmp(&headerv2->pvrTag, gPVRTexIdentifier, strlen(gPVRTexIdentifier)) == 0 || BEYONDENGINE_SWAP_INT32_BIG_TO_HOST(headerv3->version) == 0x50565203;
    }
    return ret;
}

bool CImage::IsEtc( const unsigned char * data, ssize_t /*dataLen*/ )
{
    return etc1_pkm_is_valid((etc1_byte*)data) ? true : false;
}

bool CImage::IsS3TC( const unsigned char * data,ssize_t /*dataLen*/ )
{
    S3TCTexHeader *header = (S3TCTexHeader *)data;
    return strncmp(header->fileCode, "DDS", 3) == 0;
}

bool CImage::IsATITC( const unsigned char *data, ssize_t /*dataLen*/ )
{
    ATITCTexHeader *header = (ATITCTexHeader *)data;
    return strncmp(&header->identifier[1], "KTX", 3) == 0;
}

bool CImage::SaveToFile( const TString& fileName)
{
    bool bRet = false;

    do 
    {
        //only support for Texture2D::PixelFormat::RGB888 or Texture2D::PixelFormat::RGBA8888 uncompressed data
        if (IsCompressed() || (m_renderFormat != PixelFormat::RGB888 && m_renderFormat != PixelFormat::RGBA8888))
        {
            BEATS_PRINT(_T("Image: saveToFile is only support for Texture2D::PixelFormat::RGB888 or Texture2D::PixelFormat::RGBA8888 uncompressed data for now"));
            break;
        }

        if(fileName.size() <= 4)
        {
            break;
        }
        TString strLowerCasePath = CStringHelper::GetInstance()->ToLower(fileName);
        if (TString::npos != strLowerCasePath.find(_T(".png")))
        {
            if(!SaveImageToPNG(fileName))
            {
                break;
            }
        }
        else if (TString::npos != strLowerCasePath.find(_T(".jpg")))
        {
            if(!SaveImageToJPG(fileName))
            {
                break;
            }
        }
        else if (TString::npos != strLowerCasePath.find(_T(".bmp")))
        {
            if (!SaveImageToBMP(fileName))
            {
                break;
            }
        }
        else
        {
            break;
        }

        bRet = true;
    } while (0);

    return bRet;
}