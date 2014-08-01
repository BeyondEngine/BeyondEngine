#ifndef BEATS_UTILITY_MD5_MD5_H__INCLUDE
#define BEATS_UTILITY_MD5_MD5_H__INCLUDE

#include <string>
#include <fstream>

/* MD5 declaration. */
class CMD5 
{
public:
    CMD5();
    CMD5(const void *input, size_t length);
    CMD5(const std::string &str);
    CMD5(const std::wstring &str);
    CMD5(std::ifstream &in);
    CMD5(FILE* in);

    void Update(const void *input, size_t length);
    void Update(const std::string &str);
    void Update(const std::wstring &str);
    void Update(std::ifstream &in);
    void Update(FILE* in);

    std::string ToString();
    void Reset();
    const BYTE* GetData()const;

    bool operator == (const CMD5& rRef);
    bool operator < (const CMD5& rRef);

private:
    void Update(const BYTE *input, size_t length);
    void Digest();
    void Transform(const BYTE block[64]);
    void Encode(const size_t *input, BYTE *output, size_t length);
    void Decode(const BYTE *input, size_t *output, size_t length);
    std::string BytesToHexString(const BYTE *input, size_t length);

    /* class uncopyable */
    CMD5(const CMD5&);
    CMD5& operator=(const CMD5&);

private:
    size_t m_magicNumber[4];    /* state (ABCD) */
    size_t m_bitsCount[2];    /* number of bits, modulo 2^64 (low-order word first) */
    BYTE m_inputBuffer[64];    /* input buffer */
    BYTE m_digest[16];    /* message digest */
    bool m_bDigested;        /* calculate finished ? */

    static const BYTE PADDING[64];    /* padding for calculate */
    static const char HEX[16];
    static const size_t BUFFER_SIZE = 1024 * 1024;
};

#endif