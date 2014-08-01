#include "stdafx.h"
#include "Texture.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "external/Configuration.h"
#include "TextureFormatConverter.h"
#include "Renderer.h"

static PixelFormat g_defaultAlphaPixelFormat = PixelFormat::DEFAULT;

CTexture::CTexture()
: m_bPremultipliedAlpha(false)
, m_iWidth(0)
, m_iHeight(0)
, m_uMipmapCount(0)
, m_uId(0)
, m_pixelFormat(PixelFormat::NONE)
, m_pImage(nullptr)
{
}

CTexture::~CTexture()
{
    BEATS_SAFE_DELETE( m_pImage );
}

GLuint CTexture::ID() const
{
    return m_uId;
}

bool CTexture::Load()
{
    BEATS_ASSERT(!IsLoaded(), _T("Can't Load a texture which is already loaded!"));
    bool bRet = false;
    m_pImage = new CImage;
    bRet = m_pImage->InitWithImageFile(GetFilePath());
    BEATS_ASSERT(bRet, _T("Load file %s failed!"), GetFilePath().c_str());

#ifdef _DEBUG
    if (bRet)
    {
        int imageWidth = m_pImage->GetWidth();
        int imageHeight = m_pImage->GetHeight();

        int maxTextureSize = CConfiguration::GetInstance()->GetMaxTextureSize();
        BEATS_ASSERT(imageWidth <= maxTextureSize &&  imageHeight <= maxTextureSize, 
            _T("WARNING: Image (%u x %u) is bigger than the supported %u x %u"), 
            imageWidth, imageHeight, maxTextureSize, maxTextureSize);
    }
#endif
    if (m_pImage->GetNumberOfMipmaps() == 0 && !m_pImage->IsCompressed())
    {
        unsigned char* outTempData = nullptr;
        ssize_t outTempDataLen = 0;
        m_pixelFormat = ConvertDataToFormat(m_pImage->GetData(), m_pImage->GetDataLength(), m_pImage->GetRenderFormat(), g_defaultAlphaPixelFormat, &outTempData, &outTempDataLen);

        m_pData = new CSerializer((size_t)0, NULL);
        m_pData->SetBuffer(outTempData, outTempDataLen);
        m_pData->SetReadPos(0);
        m_pData->SetWritePos(outTempDataLen);
    }

    super::Load();
    return bRet;
}

bool CTexture::InitWithMipmaps( const SMipmapInfo* mipmaps, int mipmapsNum, PixelFormat pixelFormat, int pixelsWide, int pixelsHigh )
{
    bool bRet = false;

    //the pixelFormat must be a certain value 
    BEATS_ASSERT(pixelFormat != PixelFormat::NONE && pixelFormat != PixelFormat::AUTO, _T("the \"pixelFormat\" param must be a certain value!"));
    BEATS_ASSERT(pixelsWide>0 && pixelsHigh>0, _T("Invalid size"));
    BEATS_ASSERT(mipmapsNum > 0, _T("mipmap number is less than 1"));
    BEATS_WARNING( CImage::GetPixelFormatInfoMap().find(pixelFormat) != CImage::GetPixelFormatInfoMap().end(), _T("WARNING: unsupported pixelformat: %lx"), (unsigned long)pixelFormat );

    if(CImage::GetPixelFormatInfoMap().find(pixelFormat) != CImage::GetPixelFormatInfoMap().end())
    {
        const PixelFormatInfo& info = CImage::GetPixelFormatInfoMap().at(pixelFormat);
        bool bFormatNoSupport = info.compressed && !CConfiguration::GetInstance()->SupportsPVRTC()
            && !CConfiguration::GetInstance()->SupportsETC()
            && !CConfiguration::GetInstance()->SupportsS3TC()
            && !CConfiguration::GetInstance()->SupportsATITC();
        BEATS_WARNING( !bFormatNoSupport, _T("WARNING: PVRTC/ETC images are not supported"));

        if (!bFormatNoSupport)
        {
            //Set the row align only when mipmapsNum == 1 and the data is uncompressed
            CRenderer* pRenderer = CRenderer::GetInstance();
            unsigned int packValue = 1;
            if (mipmapsNum == 1 && !info.compressed)
            {
                unsigned int bytesPerRow = pixelsWide * info.bpp / 8;
                if(bytesPerRow % 8 == 0)
                {
                    packValue = 8;
                }
                else if(bytesPerRow % 4 == 0)
                {
                    packValue = 4;
                }
                else if(bytesPerRow % 2 == 0)
                {
                    packValue = 2;
                }
            }
            pRenderer->PixelStorei(GL_UNPACK_ALIGNMENT, packValue);

            pRenderer->GenTextures(1, &m_uId);
            pRenderer->BindTexture(GL_TEXTURE_2D, m_uId);

            pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapsNum == 1 ? GL_LINEAR : GL_LINEAR_MIPMAP_NEAREST );
            pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

            BEYONDENGINE_CHECK_GL_ERROR_DEBUG(); // clean possible GL error

            // Specify OpenGL texture image
            int width = pixelsWide;
            int height = pixelsHigh;

            for (int i = 0; i < mipmapsNum; ++i)
            {
                unsigned char *data = mipmaps[i].address;
                GLsizei datalen = mipmaps[i].len;

                if (info.compressed)
                {
                    pRenderer->CompressedTexImage2D(GL_TEXTURE_2D, i, info.internalFormat, (GLsizei)width, (GLsizei)height, 0, datalen, data);
                }
                else
                {
                    pRenderer->TextureImage2D(GL_TEXTURE_2D, i, info.internalFormat, (GLsizei)width, (GLsizei)height, 0, info.format, info.type, data);
                }

                BEATS_ASSERT(!(i > 0 && (width != height || CImage::CcNextPOT(width) != width )), 
                                    _T("CTexture. WARNING. Mipmap level %u is not squared. Texture won't render correctly. width=%d != height=%d"), i, width, height);
                BEYONDENGINE_CHECK_GL_ERROR_DEBUG();

                width = MAX(width >> 1, 1);
                height = MAX(height >> 1, 1);
            }

            m_iWidth = pixelsWide;
            m_iHeight = pixelsHigh;
            m_pixelFormat = pixelFormat;

            m_bPremultipliedAlpha = false;
            m_uMipmapCount = mipmapsNum;
        }
    }
    return bRet;
}

bool CTexture::InitWithData(const void *data, ssize_t dataLen, PixelFormat pixelFormat, int pixelsWide, int pixelsHigh)
{
    BEATS_ASSERT(pixelsWide > 0 && pixelsHigh > 0, _T("Invalid size"));

    //if data has no mipmaps, we will consider it has only one mipmap
    SMipmapInfo mipmap;
    mipmap.address = (unsigned char*)data;
    mipmap.len = static_cast<int>(dataLen);
    return InitWithMipmaps(&mipmap, 1, pixelFormat, pixelsWide, pixelsHigh);
}

bool CTexture::UpdateSubImage( GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, const GLvoid *data )
{
    const PixelFormatInfo& info = CImage::GetPixelFormatInfoMap().at(m_pixelFormat);
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->BindTexture(GL_TEXTURE_2D, m_uId);
    pRenderer->TextureSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, width, height, info.format, info.type, data);
    pRenderer->BindTexture(GL_TEXTURE_2D, 0);
    return true;
}

PixelFormat CTexture::ConvertDataToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat originFormat, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{
    CTextureFormatConverter* pFormatConverter = CTextureFormatConverter::GetInstance();
    switch (originFormat)
    {
    case PixelFormat::I8:
        return pFormatConverter->convertI8ToFormat(data, dataLen, format, outData, outDataLen);
    case PixelFormat::AI88:
        return pFormatConverter->convertAI88ToFormat(data, dataLen, format, outData, outDataLen);
    case PixelFormat::RGB888:
        return pFormatConverter->convertRGB888ToFormat(data, dataLen, format, outData, outDataLen);
    case PixelFormat::RGBA8888:
        return pFormatConverter->convertRGBA8888ToFormat(data, dataLen, format, outData, outDataLen);
    default:
        BEATS_WARNING(false, _T("unsupport convert for format %d to format %d"), originFormat, format);
        *outData = (unsigned char*)data;
        *outDataLen = dataLen;
        return originFormat;
    }
}

int CTexture::Width() const
{
    return m_iWidth;
}

int CTexture::Height() const
{
    return m_iHeight;
}

bool CTexture::operator==( const CTexture& other )
{
    return m_uId == other.m_uId;
}

bool CTexture::operator!=( const CTexture& other )
{
    return m_uId != other.m_uId;
}

void CTexture::Initialize()
{
    super::Initialize();
    bool bRet = false;
    if ( m_pImage )
    {
        int imageWidth = m_pImage->GetWidth();
        int imageHeight = m_pImage->GetHeight();

        if (m_pImage->GetNumberOfMipmaps() > 1)
        {
            InitWithMipmaps(m_pImage->GetMipmaps(), m_pImage->GetNumberOfMipmaps(), (PixelFormat)m_pImage->GetRenderFormat(), imageWidth, imageHeight);
            bRet = true;
        }
        else if (m_pImage->IsCompressed())
        {
            InitWithData(m_pImage->GetData(), m_pImage->GetDataLength(), (PixelFormat)m_pImage->GetRenderFormat(), imageWidth, imageHeight);
            bRet = true;
        }
        else
        {
            InitWithData(m_pData->GetBuffer(), m_pData->GetWritePos(), m_pixelFormat, imageWidth, imageHeight);
            // set the premultiplied tag
            if (!m_pImage->HasPremultipliedAlpha())
            {
                m_bPremultipliedAlpha = m_pImage->GetFileType() != CImage::EFormat::eF_PVR;
                BEATS_WARNING( !m_bPremultipliedAlpha, _T("wanning: We cann't find the data is premultiplied or not, we will assume it's false."));
            }
            else
            {
                m_bPremultipliedAlpha = m_pImage->IsPremultipliedAlpha();
            }
            if (m_pData != NULL)
            {
                BEATS_SAFE_DELETE(m_pImage);
                m_pData->SetBuffer(NULL, 0, false);
                BEATS_SAFE_DELETE(m_pData);
            }
            bRet = true;
        }
    }
    BEATS_ASSERT(bRet, _T("Init texture %s failed!"), m_strPath.m_value.c_str());
}

void CTexture::Uninitialize()
{
    super::Uninitialize();
    if (m_uId != 0)
    {
        CRenderer::GetInstance()->DeleteTexture(1, &m_uId);
    }
}
