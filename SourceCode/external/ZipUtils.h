#ifndef CZIPUTILS_H_INCLUDE
#define CZIPUTILS_H_INCLUDE


struct SZHeader
{
    unsigned char   sig[4];             // signature. Should be 'CCZ!' 4 bytes
    unsigned short  compression_type;   // should 0
    unsigned short  version;            // should be 2 (although version type==1 is also supported)
    unsigned int    reserved;           // Reserved for users.
    unsigned int    len;                // size of the uncompressed file
};

enum EZipFormat
{
    CCZ_COMPRESSION_ZLIB,               // zlib format.
    CCZ_COMPRESSION_BZIP2,              // bzip2 format (not supported yet)
    CCZ_COMPRESSION_GZIP,               // gzip format (not supported yet)
    CCZ_COMPRESSION_NONE,               // plain (not supported yet)
};

class CZipUtils
{
public:
    CZipUtils();
    ~CZipUtils();

    static ssize_t InflateMemory(unsigned char *in, ssize_t inLength, unsigned char **out);

    static ssize_t InflateMemoryWithHint(unsigned char *in, ssize_t inLength, unsigned char **out, ssize_t outLengthHint);

    static int InflateGZipFile(const char *filepath, unsigned char **out);

    static bool IsGZipFile(const TCHAR *filePath);

    static bool IsGZipBuffer(const unsigned char *buffer, ssize_t len);

    static int InflateCCZFile(const TCHAR *filePath, unsigned char **out);

    static int InflateCCZBuffer(const unsigned char *buffer, ssize_t bufferLen, unsigned char **out);

    static bool IsCCZFile(const TCHAR *path);

    static bool IsCCZBuffer(const unsigned char *buffer, ssize_t bufferLen);

    static void SetPvrEncryptionKeyPart(int index, unsigned int value);

    static void SetPvrEncryptionKey(unsigned int keyPart1, unsigned int keyPart2, unsigned int keyPart3, unsigned int keyPart4);

    static void Compress(const unsigned char* SourceBuffer, unsigned long sourceLength, unsigned char* DestBuffer, unsigned long* DestLength);

    static unsigned long CompressBound(unsigned long uLength);

private:
    static int InflateMemoryWithHint(unsigned char *in, ssize_t inLength, unsigned char **out, ssize_t *outLength, ssize_t outLenghtHint);
    static inline void DecodeEncodedPvr (unsigned int *data, ssize_t len);
    static inline unsigned int ChecksumPvr(const unsigned int *data, ssize_t len);

    static unsigned int s_uEncryptedPvrKeyParts[4];
    static unsigned int s_uEncryptionKey[1024];
    static bool s_bEncryptionKeyIsValid;

};


#endif