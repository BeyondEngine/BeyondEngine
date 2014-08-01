#ifndef ACTITC_H_INCLUDE
#define ACTITC_H_INCLUDE

enum class EATITCDecodeFlag
{
    ATC_RGB = 1,
    ATC_EXPLICIT_ALPHA = 3,
    ATC_INTERPOLATED_ALPHA = 5,
};


class CAtitc
{
public:

    static void Atitc_decode(uint8_t *encode_data,
        uint8_t *decode_data,
        const int pixelsWidth,
        const int pixelsHeight,
        EATITCDecodeFlag decodeFlag
        );

private:

    static void Atitc_decode_block(
        uint8_t **blockData,
        uint32_t *decodeBlockData,
        unsigned int stride,
        bool oneBitAlphaFlag,
        uint64_t alpha,
        EATITCDecodeFlag decodeFlag);

};


#endif