#ifndef BEATS_UTILITY_MD5_MD5_H__INCLUDE
#define BEATS_UTILITY_MD5_MD5_H__INCLUDE

/* MD5 declaration. */
class CMD5 
{
public:
    CMD5();
    CMD5(const void *input, uint32_t length);
    CMD5(const std::string &str);
    CMD5(const std::wstring &str);
    CMD5(std::ifstream &in);
    CMD5(FILE* in);

    void Update(const void *input, uint32_t length);
    void Update(const std::string &str);
    void Update(const std::wstring &str);
    void Update(std::ifstream &in);
    void Update(FILE* in);

    std::string ToString();
    void Reset();
    const unsigned char* GetData()const;

    bool operator == (const CMD5& rRef);
    bool operator < (const CMD5& rRef);

private:
    void Update(const unsigned char *input, uint32_t length);
    void Digest();
    void Transform(const unsigned char block[64]);
    void Encode(const uint32_t *input, unsigned char *output, uint32_t length);
    void Decode(const unsigned char *input, uint32_t *output, uint32_t length);
    std::string charsToHexString(const unsigned char *input, uint32_t length);

    /* class uncopyable */
    CMD5(const CMD5&);
    CMD5& operator=(const CMD5&);

private:
    uint32_t m_magicNumber[4];    /* state (ABCD) */
    uint32_t m_bitsCount[2];    /* number of bits, modulo 2^64 (low-order word first) */
    unsigned char m_inputBuffer[64];    /* input buffer */
    unsigned char m_digest[16];    /* message digest */
    bool m_bDigested;        /* calculate finished ? */

    static const unsigned char PADDING[64];    /* padding for calculate */
    static const unsigned char HEX[16];
    static const uint32_t BUFFER_SIZE = 1024 * 1024;
};

#endif