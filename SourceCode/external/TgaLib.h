#ifndef CTAHLIN_H_INCLUDE
#define CTAHLIN_H_INCLUDE

enum ETgaReturnType
{
    TGA_OK,
    TGA_ERROR_FILE_OPEN,
    TGA_ERROR_READING_FILE,
    TGA_ERROR_INDEXED_COLOR,
    TGA_ERROR_MEMORY,
    TGA_ERROR_COMPRESSED_FILE,
};

/** TGA format */
typedef struct SImageTGA
{
    int status;
    unsigned char type, pixelDepth;

    /** map width */
    signed short width;

    /** map height */
    signed short height;

    /** raw data */
    unsigned char *imageData;
    int flipped;
    SImageTGA()
    {
        imageData = NULL;
    }
    ~SImageTGA()
    {
        BEATS_SAFE_DELETE_ARRAY( imageData );
    }
} TImageTGA;

class CTgaLib
{
public:
    CTgaLib();
    ~CTgaLib();

    /// load the image header fields. We only keep those that matter!
    static bool TgaLoadHeader( const unsigned char *buffer, unsigned long bufSize, SImageTGA *info);

    /// loads the image pixels. You shouldn't call this function directly
    static bool TgaLoadImageData( const unsigned char *buffer, unsigned long bufSize, SImageTGA *info);

    /// this is the function to call when we want to load an image buffer.
    static SImageTGA* TgaLoadBuffer( const unsigned char* buffer, long size);

    /// this is the function to call when we want to load an image
    static SImageTGA * TgaLoad(const TCHAR* pszFileName);

    // /converts RGB to grayscale
    static void TgaRGBtogreyscale(SImageTGA *info);

    /// releases the memory used for the image
    static void TgaDestroy(SImageTGA *info);

    static bool TgaLoadRLEImageData( const unsigned char* buffer, unsigned long bufSize, TImageTGA *info);

    static void TgaFlipImage( TImageTGA *info );

};

#endif//CTAHLIN_H_INCLUDE