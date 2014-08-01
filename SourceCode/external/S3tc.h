#ifndef CS3TC_H_INCLUDE
#define CS3TC_H_INCLUDE


enum class ES3TCDecodeFlag
{
    DXT1 = 1,
    DXT3 = 3,
    DXT5 = 5,
};

class CS3tc
{
public:
   

    static void S3tc_decode( 
        uint8_t *encode_data,
        uint8_t *decode_data,
        const int pixelsWidth,
        const int pixelsHeight,
        ES3TCDecodeFlag decodeFlag);

private:
    static void S3tc_decode_block(
        uint8_t **blockData,
        uint32_t *decodeBlockData,
        unsigned int stride,
        bool oneBitAlphaFlag,
        uint64_t alpha,
        ES3TCDecodeFlag decodeFlag);
};


#endif // !CS3TC_H_INCLUDE
