#ifndef BEYOND_ENGINE_RENDER_TEXTURE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_TEXTURE_H__INCLUDE

#include "Resource/Resource.h"
#include "RenderPublic.h"
#include "external/Image.h"

class CTexture : public CResource
{
    DECLARE_REFLECT_GUID(CTexture, 0x32D9C0E4, CResource)
    DECLARE_RESOURCE_TYPE(eRT_Texture)
public:
    CTexture();
    virtual ~CTexture();

    virtual bool Load() override;
    virtual void Initialize() override;
    virtual void Uninitialize() override;

    GLuint ID() const;

    bool InitWithMipmaps( const SMipmapInfo* mipmaps, int mipmapsNum, PixelFormat pixelFormat, int pixelsWide, int pixelsHigh);
    bool InitWithData(const void *data, ssize_t dataLen, PixelFormat pixelFormat, int pixelsWide, int pixelsHigh);
    bool UpdateSubImage(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, const GLvoid *data);
    PixelFormat ConvertDataToFormat(const unsigned char* data, 
                                                        ssize_t dataLen, 
                                                        PixelFormat originFormat,
                                                        PixelFormat format,
                                                        unsigned char** outData, 
                                                        ssize_t* outDataLen);

    int Width() const;
    int Height() const;

    bool operator==( const CTexture& other );

    bool operator!=( const CTexture& other );


private:
    bool m_bPremultipliedAlpha;
    int m_iWidth;
    int m_iHeight;
    size_t m_uMipmapCount;
    GLuint m_uId;
    PixelFormat m_pixelFormat;
    CImage* m_pImage;
};

#endif