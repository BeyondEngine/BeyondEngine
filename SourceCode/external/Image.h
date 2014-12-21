#ifndef CIMAGET_H_INCLUDE
#define CIMAGET_H_INCLUDE

#define BEYOND_ENGINE_RGB_PREMULTIPLY_ALPHA(vr, vg, vb, va) \
    (unsigned)(((unsigned)((unsigned char)(vr) * ((unsigned char)(va) + 1)) >> 8) | \
    ((unsigned)((unsigned char)(vg) * ((unsigned char)(va) + 1) >> 8) << 8) | \
    ((unsigned)((unsigned char)(vb) * ((unsigned char)(va) + 1) >> 8) << 16) | \
    ((unsigned)(unsigned char)(va) << 24))

typedef struct SMipmapInfo
{
    unsigned char* address;
    int len;
}TMipmapInfo;

typedef struct ImageSource
{
    const unsigned char * data;
    ssize_t size;
    int offset;
}TImageSource;

typedef struct  SImageTGA TImageTGA;

class CImage
{
public:
    CImage();
    ~CImage();

    enum EFormat
    {
        //! JPEG
        eF_JPG,
        //! PNG
        eF_PNG,
        //! TIFF
        eF_TIFF,
        //! WebP
        eF_WEBP,
        //! PVR
        eF_PVR,
        //! ETC
        eF_ETC,
        //! S3TC
        eF_S3TC,
        //! ATITC
        eF_ATITC,
        //! TGA
        eF_TGA,
        //! Raw Data
        eF_RAW_DATA,
        //! Unknown format
        eF_UNKOWN
    };

    bool InitWithImageData( const unsigned char* data, ssize_t dataLen, const TCHAR* filename);

    bool InitWithRawData(const unsigned char * data, ssize_t dataLen, int width, int height, int bitsPerComponent, const TCHAR* filename, bool preMulti = false);

    const unsigned char* GetData() const;
    void SetData(PixelFormat renderFormat, unsigned char* pData, ssize_t len);
    ssize_t GetDataLength() const;
    EFormat GetFileType() const;
    PixelFormat GetRenderFormat() const;
    int GetWidth() const;
    int GetHeight() const;
    bool IsPremultipliedAlpha() const;
    int GetNumberOfMipmaps() const;
    const TMipmapInfo* GetMipmaps() const;
    bool HasPremultipliedAlpha() const;

    int GetBitPerPixel();
    void SetRGB(int32_t x, int32_t y, unsigned char r, unsigned char g, unsigned char b);
    unsigned char GetAlpha(int32_t x, int32_t y) const;
    bool HasAlpha() const;
    bool IsCompressed() const;
    bool SaveToFile( const TString& fileName);
    static const PixelFormatInfoMap& GetPixelFormatInfoMap();

    static int CcNextPOT(int x);

protected:
    bool InitWithJpgData(const unsigned char *  data, ssize_t dataLen, const TCHAR* filename);
    bool InitWithPngData(const unsigned char * data, ssize_t dataLen, const TCHAR* filename);
    bool InitWithTiffData(const unsigned char * data, ssize_t dataLen, const TCHAR* filename);
    bool InitWithWebpData(const unsigned char * data, ssize_t dataLen, const TCHAR* filename);
    bool InitWithPVRData(const unsigned char * data, ssize_t dataLen, const TCHAR* filename);
    bool InitWithPVRv2Data(const unsigned char * data, ssize_t dataLen, const TCHAR* filename);
    bool InitWithPVRv3Data(const unsigned char * data, ssize_t dataLen, const TCHAR* filename);
    bool InitWithETCData(const unsigned char * data, ssize_t dataLen, const TCHAR* filename);
    bool InitWithS3TCData(const unsigned char * data, ssize_t dataLen, const TCHAR* filename);
    bool InitWithATITCData(const unsigned char *data, ssize_t dataLen, const TCHAR* filename);
    bool InitWithTGAData(TImageTGA* tgaData, const char* filename);
    bool SaveImageToPNG(const TString& filePath);
    bool SaveImageToJPG(const TString& filePath);
    bool SaveImageToBMP(const TString& filePath);

protected:
    // noncopyable
    CImage(const CImage&    rImg);
    CImage& operator=(const CImage&);

    /*
     @brief The same result as with initWithImageFile, but thread safe. It is caused by
     loadImage() in TextureCache.cpp.
     @param fullpath  full path of the file.
     @param imageType the type of image, currently only supporting two types.
     @return  true if loaded correctly.
     */
    bool InitWithImageFileThreadSafe(const TString& fullpath);

    EFormat DetectFormat(const unsigned char * data, ssize_t dataLen);
    bool IsPng(const unsigned char * data, ssize_t dataLen);
    bool IsJpg(const unsigned char * data, ssize_t dataLen);
    bool IsTiff(const unsigned char * data, ssize_t dataLen);
    bool IsWebp(const unsigned char * data, ssize_t dataLen);
    bool IsPvr(const unsigned char * data, ssize_t dataLen);
    bool IsEtc(const unsigned char * data, ssize_t dataLen);
    bool IsS3TC(const unsigned char * data,ssize_t dataLen);
    bool IsATITC(const unsigned char *data, ssize_t dataLen);

protected:
    /**
     @brief Determine how many mipmaps can we have.
     Its same as define but it respects namespaces
     */
    static const int MIPMAP_MAX = 16;
    unsigned char *m_data;
    ssize_t m_dataLen;
    int m_width;
    int m_height;
    EFormat m_fileType;
    PixelFormat m_renderFormat;
    bool m_bUnpack = false;
    bool m_preMulti;
    TMipmapInfo m_mipmaps[MIPMAP_MAX];   // pointer to mipmap images
    int m_numberOfMipmaps;
    // false if we cann't auto detect the image is premultiplied or not.
    bool m_hasPremultipliedAlpha;
    TString m_filePath;

    static const PixelFormatInfoMap _pixelFormatInfoTables;
};

#endif//CIMAGET_H_INCLUDE
