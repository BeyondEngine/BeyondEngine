#include "stdafx.h"
#include "ZipUtils.h"
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
#include "zlib/include/zlib.h"
#else
#include "zlib.h"
#endif

// memory in iPhone is precious
// Should buffer factor be 1.5 instead of 2 ?
#define BUFFER_INC_FACTOR (2)

unsigned int CZipUtils::s_uEncryptedPvrKeyParts[4] = {0,0,0,0};
unsigned int CZipUtils::s_uEncryptionKey[1024];
bool CZipUtils::s_bEncryptionKeyIsValid = false;

CZipUtils::CZipUtils()
{

}

CZipUtils::~CZipUtils()
{

}

ssize_t CZipUtils::InflateMemory( unsigned char *in, ssize_t inLength, unsigned char **out )
{
    return InflateMemoryWithHint(in, inLength, out, 256 * 1024);
}

ssize_t CZipUtils::InflateMemoryWithHint( unsigned char *in, ssize_t inLength, unsigned char **out, ssize_t outLengthHint )
{
    ssize_t outLength = 0;
    int err = InflateMemoryWithHint(in, inLength, out, &outLength, outLengthHint);

    if (err != Z_OK || *out == NULL)
    {
        if (err == Z_MEM_ERROR)
        {
            BEATS_PRINT(_T("ZipUtils: Out of memory while decompressing map data!"));
        }
        else if (err == Z_VERSION_ERROR)
        {
            BEATS_PRINT(_T("ZipUtils: Incompatible zlib version!"));
        }
        else if (err == Z_DATA_ERROR)
        {
            BEATS_PRINT(_T("ZipUtils: Incorrect zlib compressed data!"));
        }
        else
        {
            BEATS_PRINT(_T("cocos2d: ZipUtils: Unknown error while decompressing map data!"));
        }

        if(*out)
        {
            BEATS_SAFE_DELETE_ARRAY(*out);
            *out = NULL;
        }
        outLength = 0;
    }

    return outLength;
}

int CZipUtils::InflateMemoryWithHint( unsigned char *in, ssize_t inLength, unsigned char **out, ssize_t *outLength, ssize_t outLenghtHint )
{
    /* ret value */
    int err = Z_OK;

    ssize_t bufferSize = outLenghtHint;
    *out = new unsigned char[bufferSize];

    z_stream d_stream; /* decompression stream */
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in  = in;
    d_stream.avail_in = static_cast<unsigned int>(inLength);
    d_stream.next_out = *out;
    d_stream.avail_out = static_cast<unsigned int>(bufferSize);

    /* window size to hold 256k */
    if( (err = inflateInit2(&d_stream, 15 + 32)) != Z_OK )
        return err;
    bool bReturn = false;

    while (true)
    {
        err = inflate(&d_stream, Z_NO_FLUSH);

        if (err == Z_STREAM_END)
        {
            break;
        }

        switch (err)
        {
        case Z_NEED_DICT:
            {
                err = Z_DATA_ERROR;
                break;
            }
            
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            {
                inflateEnd(&d_stream);
                bReturn = true;
                break;
            }
        }

        if ( bReturn )
        {
            break;
        }
        
        // not enough memory ?
        if (err != Z_STREAM_END)
        {
            unsigned char* newBuffer = new unsigned char[bufferSize * BUFFER_INC_FACTOR];
            /* not enough memory, ouch */
            if (! newBuffer )
            {
                BEATS_PRINT(_T("ZipUtils: realloc failed"));
                inflateEnd(&d_stream);
                err = Z_MEM_ERROR;
                break;
            }
            memset( newBuffer, 0, bufferSize * BUFFER_INC_FACTOR );
            memcpy( newBuffer, *out, bufferSize);
            BEATS_SAFE_DELETE_ARRAY( *out );
            *out = newBuffer;
            
            d_stream.next_out = *out + bufferSize;
            d_stream.avail_out = static_cast<unsigned int>(bufferSize);
            bufferSize *= BUFFER_INC_FACTOR;
        }
    }

    *outLength = bufferSize - d_stream.avail_out;
    err = inflateEnd(&d_stream);
    return err;
}

int CZipUtils::InflateGZipFile( const char *filepath, unsigned char **out )
{
    int returnInt = -1;
    int len = 0;
    unsigned int offset = 0;

    BEATS_ASSERT( out );
    BEATS_ASSERT( &*out );

    gzFile inFile = gzopen(filepath, "rb");
    if( inFile != NULL )
    {
        /* 512k initial decompress buffer */
        unsigned int bufferSize = 512 * 1024;
        unsigned int totalBufferSize = bufferSize;

        *out = new unsigned char[ bufferSize ];
        if( ! out )
        {
            while (true)
            {
                len = gzread(inFile, *out + offset, bufferSize);
                if (len < 0)
                {
                    BEATS_PRINT(_T("ZipUtils: error in gzread"));
                    BEATS_SAFE_DELETE_ARRAY( *out );
                    *out = NULL;
                    break;
                }
                if (len == 0)
                {
                    break;
                }
                offset += len;
                // finish reading the file
                if( (unsigned int)len < bufferSize )
                {
                    break;
                }
                bufferSize *= BUFFER_INC_FACTOR;
                totalBufferSize += bufferSize;
                unsigned char* newBuffer = new unsigned char[ totalBufferSize ];
                if( ! newBuffer )
                {
                    BEATS_PRINT(_T(" ZipUtils: out of memory"));
                    BEATS_SAFE_DELETE_ARRAY( *out );
                    break;
                }
                memset( newBuffer, 0, totalBufferSize );
                memcpy( newBuffer, *out, bufferSize );
                BEATS_SAFE_DELETE_ARRAY( *out );
                *out = newBuffer;
            }

            if (gzclose(inFile) != Z_OK)
            {
                BEATS_PRINT(_T("ZipUtils: gzclose failed"));
            }
        }
        else
        {
            BEATS_PRINT(_T("ZipUtils: out of memory"));
        }
    }
    else
    {
        BEATS_PRINT(_T("ZipUtils: error open gzip file: %s"), filepath);
    }
    returnInt = offset;

    return returnInt;
}

bool CZipUtils::IsGZipFile( const TCHAR *filePath )
{
    bool bReturn = false;
    CSerializer serializer(filePath);
    if (serializer.GetWritePos() > 0)
    {
        bReturn = IsGZipBuffer(serializer.GetBuffer(), serializer.GetWritePos());
    }
    return bReturn;
}

bool CZipUtils::IsGZipBuffer( const unsigned char *buffer, ssize_t len )
{
    bool bReturn = false;
    if (len > 2)
    {
        bReturn = buffer[0] == 0x1F && buffer[1] == 0x8B;
    }
    return bReturn;
}

int CZipUtils::InflateCCZFile( const TCHAR *filePath, unsigned char **out )
{
    BEATS_ASSERT( out, _T("Invalid pointer for buffer!"));
    int returnInt= -1;
    // load file into memory
    CSerializer serializer(filePath);
    BEATS_ASSERT(serializer.GetWritePos() > 0, _T("Error loading CCZ compressed file"));
    if (serializer.GetWritePos() > 0)
    {
        returnInt = InflateCCZBuffer(serializer.GetBuffer(), serializer.GetWritePos(), out);
    }
    return returnInt;
}

int CZipUtils::InflateCCZBuffer( const unsigned char *buffer, ssize_t bufferLen, unsigned char **out )
{
    int returnInt = -1;
    SZHeader *header = (SZHeader*) buffer;

    do
    {
        // verify header
        if( header->sig[0] == 'C' && header->sig[1] == 'C' && header->sig[2] == 'Z' && header->sig[3] == '!' )
        {
            // verify header version
            unsigned int version = BEYONDENGINE_SWAP_INT16_BIG_TO_HOST( header->version );
            if( version > 2 )
            {
                BEATS_PRINT(_T("Unsupported CCZ header format"));
                break;
            }

            // verify compression format
            if( BEYONDENGINE_SWAP_INT16_BIG_TO_HOST(header->compression_type) != CCZ_COMPRESSION_ZLIB )
            {
                BEATS_PRINT(_T("CCZ Unsupported compression method"));
                break;
            }
        }
        else if( header->sig[0] == 'C' && header->sig[1] == 'C' && header->sig[2] == 'Z' && header->sig[3] == 'p' )
        {
            // encrypted ccz file
            header = (SZHeader*) buffer;

            // verify header version
            unsigned int version = BEYONDENGINE_SWAP_INT16_BIG_TO_HOST( header->version );
            if( version > 0 )
            {
                BEATS_PRINT(_T("Unsupported CCZ header format"));
                break;
            }

            // verify compression format
            if( BEYONDENGINE_SWAP_INT16_BIG_TO_HOST(header->compression_type) != CCZ_COMPRESSION_ZLIB )
            {
                BEATS_PRINT(_T("CCZ Unsupported compression method"));
                break;
            }

            // decrypt
            unsigned int* ints = (unsigned int*)(buffer+12);
            ssize_t enclen = (bufferLen-12)/4;

            DecodeEncodedPvr(ints, enclen);

#ifdef _DEBUG
            // verify checksum in debug mode
            unsigned int calculated = ChecksumPvr(ints, enclen);
            unsigned int required = BEYONDENGINE_SWAP_INT32_BIG_TO_HOST( header->reserved );

            if(calculated != required)
            {
                BEATS_PRINT(_T("Can't decrypt image file. Is the decryption key valid?"));
                break;
            }
#endif
        }
        else
        {
            BEATS_PRINT(_T("Invalid CCZ file"));
            break;
        }

        unsigned int len = BEYONDENGINE_SWAP_INT32_BIG_TO_HOST( header->len );

        *out = new unsigned char[ len ];
        if(! *out )
        {
            BEATS_PRINT(_T("CCZ: Failed to allocate memory for texture"));
            break;
        }

        unsigned long destlen = len;
        size_t source = (size_t) buffer + sizeof(*header);
        int ret = uncompress(*out, &destlen, (Bytef*)source, bufferLen - sizeof(*header) );

        if( ret != Z_OK )
        {
            BEATS_PRINT(_T("CCZ: Failed to uncompress data"));
            BEATS_SAFE_DELETE_ARRAY( *out );
            break;
        }
        returnInt = len;

    } while ( 0 );
    return returnInt;
}

bool CZipUtils::IsCCZFile( const TCHAR *path )
{
    bool bReturn = false;
    // load file into memory
    CSerializer serializer(path);
    BEATS_ASSERT(serializer.GetWritePos() > 0, _T("ZipUtils: loading file failed"));
    if (serializer.GetWritePos() > 0)
    {
        bReturn = IsCCZBuffer(serializer.GetBuffer(), serializer.GetWritePos());
    }
    return bReturn;
}

bool CZipUtils::IsCCZBuffer( const unsigned char *buffer, ssize_t bufferLen )
{
    bool bReturn = false;
    if (static_cast<size_t>(bufferLen) >= sizeof(SZHeader))
    {
        SZHeader *header = (SZHeader*) buffer;
        bReturn = header->sig[0] == 'C' && header->sig[1] == 'C' && header->sig[2] == 'Z' && (header->sig[3] == '!' || header->sig[3] == 'p');
    }
    return bReturn;
}

void CZipUtils::SetPvrEncryptionKeyPart( int index, unsigned int value )
{
    BEATS_ASSERT(index >= 0, _T("key part index cannot be less than 0"));
    BEATS_ASSERT(index <= 3, _T("key part index cannot be greater than 3"));

    if(s_uEncryptedPvrKeyParts[index] != value)
    {
        s_uEncryptedPvrKeyParts[index] = value;
        s_bEncryptionKeyIsValid = false;
    }
}

void CZipUtils::SetPvrEncryptionKey( unsigned int keyPart1, unsigned int keyPart2, unsigned int keyPart3, unsigned int keyPart4 )
{
    SetPvrEncryptionKeyPart(0, keyPart1);
    SetPvrEncryptionKeyPart(1, keyPart2);
    SetPvrEncryptionKeyPart(2, keyPart3);
    SetPvrEncryptionKeyPart(3, keyPart4);
}

void CZipUtils::DecodeEncodedPvr( unsigned int *data, ssize_t len )
{
    const int enclen = 1024;
    const int securelen = 512;
    const int distance = 64;

    // check if key was set
    // make sure to call caw_setkey_part() for all 4 key parts
    BEATS_ASSERT(s_uEncryptedPvrKeyParts[0] != 0, _T("CCZ file is encrypted but key part 0 is not set. Did you call ZipUtils::setPvrEncryptionKeyPart(...)?"));
    BEATS_ASSERT(s_uEncryptedPvrKeyParts[1] != 0, _T("CCZ file is encrypted but key part 1 is not set. Did you call ZipUtils::setPvrEncryptionKeyPart(...)?"));
    BEATS_ASSERT(s_uEncryptedPvrKeyParts[2] != 0, _T("CCZ file is encrypted but key part 2 is not set. Did you call ZipUtils::setPvrEncryptionKeyPart(...)?"));
    BEATS_ASSERT(s_uEncryptedPvrKeyParts[3] != 0, _T("CCZ file is encrypted but key part 3 is not set. Did you call ZipUtils::setPvrEncryptionKeyPart(...)?"));

    // create long key
    if(!s_bEncryptionKeyIsValid)
    {
        unsigned int y, p, e;
        unsigned int rounds = 6;
        unsigned int sum = 0;
        unsigned int z = s_uEncryptionKey[enclen-1];

        do
        {
#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (s_uEncryptedPvrKeyParts[(p&3)^e] ^ z)))

            sum += DELTA;
            e = (sum >> 2) & 3;

            for (p = 0; p < enclen - 1; p++)
            {
                y = s_uEncryptionKey[p + 1];
                z = s_uEncryptionKey[p] += MX;
            }

            y = s_uEncryptionKey[0];
            z = s_uEncryptionKey[enclen - 1] += MX;

        } while (--rounds);

        s_bEncryptionKeyIsValid = true;
    }

    int b = 0;
    int i = 0;

    // encrypt first part completely
    for(; i < len && i < securelen; i++)
    {
        data[i] ^= s_uEncryptionKey[b++];

        if(b >= enclen)
        {
            b = 0;
        }
    }

    // encrypt second section partially
    for(; i < len; i += distance)
    {
        data[i] ^= s_uEncryptionKey[b++];

        if(b >= enclen)
        {
            b = 0;
        }
    }
}

unsigned int CZipUtils::ChecksumPvr( const unsigned int *data, ssize_t len )
{
    unsigned int cs = 0;
    const int cslen = 128;

    len = (len < cslen) ? len : cslen;

    for(int i = 0; i < len; i++)
    {
        cs = cs ^ data[i];
    }

    return cs;
}

void CZipUtils::Compress(const unsigned char* SourceBuffer, unsigned long sourceLength, unsigned char* pDestBuffer, unsigned long* DestLength)
{
    bool bRet = compress(pDestBuffer, DestLength, SourceBuffer, sourceLength) == Z_OK;
    BEYONDENGINE_UNUSED_PARAM(bRet);
    BEATS_ASSERT(bRet, _T("压缩失败"));
}

unsigned long CZipUtils::CompressBound(unsigned long uLength)
{
    return compressBound(uLength);
}
