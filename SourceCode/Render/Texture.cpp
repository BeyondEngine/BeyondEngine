#include "stdafx.h"
#include "Texture.h"
#include "external/Configuration.h"
#include "TextureFormatConverter.h"
#include "Renderer.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Resource/ResourceManager.h"
#include "external/ZipUtils.h"
#include "RenderManager.h"

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
    if (IsInitialized())
    {
        Uninitialize();
    }
    BEATS_SAFE_DELETE(m_pEtcAlphaTexture);
}

GLuint CTexture::ID() const
{
    GLuint uRet = m_uId;
#ifdef DEVELOP_VERSION
    if (!IsLoaded())
    {
        uRet = CRenderManager::GetInstance()->GetTempTexture()->ID();
    }
#endif
    return uRet;
}

CTexture* CTexture::GetAlphaTexture() const
{
    return m_pEtcAlphaTexture;
}

bool CTexture::Load()
{
    BEATS_ASSERT(!IsLoaded(), _T("Can't Load a texture which is already loaded!"));
    const TString& strFilePath = GetFilePath();
    BEATS_ASSERT(strFilePath.length() > 0, _T("Can't load an image with empty path!"));
    bool bRet = CFilePathTool::GetInstance()->Exists(strFilePath.c_str());
    BEATS_ASSERT(bRet, _T("File %s doesn't exists!"), strFilePath.c_str());
    if (bRet)
    {
        CSerializer filedata(strFilePath.c_str());
        CResourceManager::GetInstance()->DecodeResourceData(filedata);
        uint32_t uBufferSize = filedata.GetWritePos();

        if (strcmp((char*)filedata.GetReadPtr(), BEYONDENGINE_ZIP_FILE_HEADER) == 0)
        {
            TString strZipHead;
            filedata >> strZipHead;
            unsigned char* pBuffer = nullptr;
            uBufferSize = uBufferSize - sizeof(BEYONDENGINE_ZIP_FILE_HEADER);
            uBufferSize = CZipUtils::InflateMemory((unsigned char*)filedata.GetReadPtr(), uBufferSize, &pBuffer);
            BEATS_ASSERT(uBufferSize > 0);
            filedata.SetBuffer(pBuffer, uBufferSize);
            filedata.SetWritePos(uBufferSize);
        }
        CImage* pImage = new CImage;
        bRet = pImage->InitWithImageData(filedata.GetBuffer(), uBufferSize, strFilePath.c_str());
        BEATS_ASSERT(bRet, _T("Load file %s failed!"), GetFilePath().c_str());
        if (bRet)
        {
            LoadWithImage(pImage);
            if (pImage->GetFileType() == CImage::eF_ETC)
            {
                uint32_t uOriginalFileSize = etc1_get_encoded_data_size(pImage->GetWidth(), pImage->GetHeight()) + ETC_PKM_HEADER_SIZE;
                bool bContainAlphaData = uBufferSize == uOriginalFileSize * 2;
                if (bContainAlphaData)
                {
                    BEATS_ASSERT(uBufferSize % 2 == 0);
                    uBufferSize = uBufferSize / 2;
                    m_pEtcAlphaTexture = new CTexture;
                    CImage* pAlphaImage = new CImage;
                    filedata.SetReadPos(uBufferSize);
                    pAlphaImage->InitWithImageData((const unsigned char*)filedata.GetReadPtr(), uBufferSize, strFilePath.c_str());
                    m_pEtcAlphaTexture->LoadWithImage(pAlphaImage);
                    m_pEtcAlphaTexture->SetLoadFlag(true);
                }
            }
        }
    }
    return bRet && super::Load();
}

bool CTexture::InitWithMipmaps( const SMipmapInfo* mipmaps, int mipmapsNum, PixelFormat pixelFormat, int pixelsWide, int pixelsHigh )
{
    bool bRet = false;

    //the pixelFormat must be a certain value 
    BEATS_ASSERT(pixelFormat != PixelFormat::NONE && pixelFormat != PixelFormat::AUTO, _T("the \"pixelFormat\" param must be a certain value!"));
    BEATS_ASSERT(pixelsWide>0 && pixelsHigh>0, _T("Invalid size"));
    BEATS_ASSERT(mipmapsNum > 0, _T("mipmap number is less than 1"));
    BEATS_WARNING(CImage::GetPixelFormatInfoMap().find(pixelFormat) != CImage::GetPixelFormatInfoMap().end(), _T("WARNING: unsupported pixelformat: %x"), (uint32_t)pixelFormat);

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
            bRet = true;
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
    auto iter = CImage::GetPixelFormatInfoMap().find(m_pixelFormat);
    BEATS_ASSERT(iter != CImage::GetPixelFormatInfoMap().end());
    const PixelFormatInfo& info = iter->second;
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
    int nRet = m_iWidth;
#ifdef DEVELOP_VERSION
    if (!IsLoaded())
    {
        nRet = CRenderManager::GetInstance()->GetTempTexture()->Width();
    }
#endif
    return nRet;
}

int CTexture::Height() const
{
    int nRet = m_iHeight;
#ifdef DEVELOP_VERSION
    if (!IsLoaded())
    {
        nRet = CRenderManager::GetInstance()->GetTempTexture()->Height();
    }
#endif
    return nRet;
}

CImage::EFormat CTexture::GetFileFormat() const
{
    return m_fileFormat;
}

#ifdef DEVELOP_VERSION
TString CTexture::GetDescription() const
{
    TString strRet = super::GetDescription();
    TCHAR szBuffer[256];
    _stprintf(szBuffer, _T("Size: %d x %d, Pixel Format: %d, "), m_iWidth, m_iHeight, m_pixelFormat);
    strRet.append(szBuffer);
    return strRet;
}
#endif
#ifdef EDITOR_MODE
void CTexture::Reload()
{
    super::Reload();
    Uninitialize();
    Initialize();
}
#endif

void CTexture::LoadWithImage(CImage* pImage)
{
    BEATS_ASSERT(pImage != nullptr && m_pImage == nullptr);
    m_pImage = pImage;
    m_iWidth = m_pImage->GetWidth();
    m_iHeight = m_pImage->GetHeight();

#ifdef _DEBUG
    int maxTextureSize = CConfiguration::GetInstance()->GetMaxTextureSize();
    BEATS_ASSERT(m_iWidth <= maxTextureSize &&  m_iHeight <= maxTextureSize,
        _T("WARNING: Image (%u x %u) is bigger than the supported %u x %u"),
        m_iWidth, m_iHeight, maxTextureSize, maxTextureSize);
#endif
    unsigned char* outTempData = nullptr;
    ssize_t outTempDataLen = 0;
    m_pixelFormat = pImage->GetRenderFormat();
    if (m_pImage->GetNumberOfMipmaps() == 0 && !m_pImage->IsCompressed())
    {
        m_pixelFormat = ConvertDataToFormat(m_pImage->GetData(), m_pImage->GetDataLength(), m_pImage->GetRenderFormat(), g_defaultAlphaPixelFormat, &outTempData, &outTempDataLen);
        if (m_pImage->GetData() != outTempData)
        {
            BEATS_ASSERT(m_pixelFormat != pImage->GetRenderFormat());
            m_pImage->SetData(m_pixelFormat, outTempData, outTempDataLen);
        }
    }
}

void CTexture::Initialize()
{
    super::Initialize();
    bool bRet = m_pImage != nullptr;
    if (bRet)
    {
        m_fileFormat = m_pImage->GetFileType();
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
            InitWithData(m_pImage->GetData(), m_pImage->GetDataLength(), m_pixelFormat, imageWidth, imageHeight);
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
            bRet = true;
        }
        if (m_pImage != NULL)
        {
            BEATS_SAFE_DELETE(m_pImage);
        }
    }
    BEATS_ASSERT(bRet, _T("Init texture %s failed!"), m_strPath.m_value.c_str());
    if (m_pEtcAlphaTexture)
    {
        m_pEtcAlphaTexture->Initialize();
    }
}

void CTexture::Uninitialize()
{
    if (IsInitialized())
    {
        if (m_uId != 0)
        {
            CRenderer::GetInstance()->DeleteTexture(1, &m_uId);
            m_uId = 0;
        }
        if (m_pEtcAlphaTexture)
        {
            m_pEtcAlphaTexture->Uninitialize();
        }
        super::Uninitialize();
    }
}
