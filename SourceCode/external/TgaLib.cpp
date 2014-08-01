#include "stdafx.h"
#include "TgaLib.h"

CTgaLib::CTgaLib()
{
}

CTgaLib::~CTgaLib()
{
}

bool CTgaLib::TgaLoadHeader( const unsigned char *buffer, unsigned long bufSize, SImageTGA *info )
{
    bool ret = false;

    do 
    {
        size_t step = sizeof(unsigned char) * 2;
        if((step + sizeof(unsigned char)) > bufSize)
        {
            break;
        }
        memcpy(&info->type, buffer + step, sizeof(unsigned char));

        step += sizeof(unsigned char) * 2;
        step += sizeof(signed short) * 4;
        if((step + sizeof(signed short) * 2 + sizeof(unsigned char)) > bufSize)
        {
            break;
        }
        memcpy(&info->width, buffer + step, sizeof(signed short));
        memcpy(&info->height, buffer + step + sizeof(signed short), sizeof(signed short));
        memcpy(&info->pixelDepth, buffer + step + sizeof(signed short) * 2, sizeof(unsigned char));

        step += sizeof(unsigned char);
        step += sizeof(signed short) * 2;
        if((step + sizeof(unsigned char)) > bufSize)
        {
            break;
        }
        unsigned char cGarbage;
        memcpy(&cGarbage, buffer + step, sizeof(unsigned char));

        info->flipped = 0;
        if ( cGarbage & 0x20 ) 
        {
            info->flipped = 1;
        }
        ret = true;
    } while (0);

    return ret;
}

bool CTgaLib::TgaLoadImageData( const unsigned char *buffer, unsigned long bufSize, SImageTGA *info )
{
    bool ret = false;
    do 
    {
        int mode,total,i;
        unsigned char aux;
        size_t step = (sizeof(unsigned char) + sizeof(signed short)) * 6;

        // mode equal the number of components for each pixel
        mode = info->pixelDepth / 8;
        // total is the number of unsigned chars we'll have to read
        total = info->height * info->width * mode;

        size_t dataSize = sizeof(unsigned char) * total;
        if((step + dataSize) > bufSize)
        {
            break;
        }
        memcpy(info->imageData, buffer + step, dataSize);

        // mode=3 or 4 implies that the image is RGB(A). However TGA
        // stores it as BGR(A) so we'll have to swap R and B.
        if (mode >= 3)
        {
            for (i=0; i < total; i+= mode)
            {
                aux = info->imageData[i];
                info->imageData[i] = info->imageData[i+2];
                info->imageData[i+2] = aux;
            }
        }

        ret = true;
    } while (0);

    return ret;
}

SImageTGA* CTgaLib::TgaLoadBuffer( const unsigned char* buffer, long size )
{
    int mode,total;
    TImageTGA* pReturn = NULL;
    do
    {
        if(! buffer)
        {
            break;
        }
        pReturn = new TImageTGA();

        // get the file header info
        if (! TgaLoadHeader(buffer, size, pReturn))
        {
            pReturn->status = TGA_ERROR_MEMORY;
            break;
        }

        // check if the image is color indexed
        if (pReturn->type == 1)
        {
            pReturn->status = TGA_ERROR_INDEXED_COLOR;
            break;
        }

        // check for other types (compressed images)
        if ((pReturn->type != 2) && (pReturn->type !=3) && (pReturn->type !=10) )
        {
            pReturn->status = TGA_ERROR_COMPRESSED_FILE;
            break;
        }

        // mode equals the number of image components
        mode = pReturn->pixelDepth / 8;
        // total is the number of unsigned chars to read
        total = pReturn->height * pReturn->width * mode;
        // allocate memory for image pixels
        pReturn->imageData = new unsigned char[ total ];

        // check to make sure we have the memory required
        if (pReturn->imageData == NULL)
        {
            pReturn->status = TGA_ERROR_MEMORY;
            break;
        }

        bool bLoadImage = false;
        // finally load the image pixels
        if ( pReturn->type == 10 )
        {
            bLoadImage = TgaLoadRLEImageData(buffer, size, pReturn);
        }
        else
        {
            bLoadImage = TgaLoadImageData(buffer, size, pReturn);
        }

        // check for errors when reading the pixels
        if (! bLoadImage)
        {
            pReturn->status = TGA_ERROR_READING_FILE;
            break;
        }
        pReturn->status = TGA_OK;

        if ( pReturn->flipped )
        {
            TgaFlipImage( pReturn );
            if ( pReturn->flipped )
            {
                pReturn->status = TGA_ERROR_MEMORY;
            }
        }
    } while(0);

    return pReturn;
}

bool CTgaLib::TgaLoadRLEImageData( const unsigned char* buffer, unsigned long bufSize, TImageTGA *info )
{
    unsigned int mode,total,i, index = 0;
    unsigned char aux[4], runlength = 0;
    unsigned int skip = 0, flag = 0;
    size_t step = (sizeof(unsigned char) + sizeof(signed short)) * 6;

    // mode equal the number of components for each pixel
    mode = info->pixelDepth / 8;
    // total is the number of unsigned chars we'll have to read
    total = info->height * info->width;

    for( i = 0; i < total; i++ )
    {
        // if we have a run length pending, run it
        if ( runlength != 0 )
        {
            // we do, update the run length count
            runlength--;
            skip = (flag != 0);
        }
        else
        {
            // otherwise, read in the run length token
            if((step + sizeof(unsigned char)) > bufSize)
            {
                break;
            }
            memcpy(&runlength, buffer + step, sizeof(unsigned char));
            step += sizeof(unsigned char);

            // see if it's a RLE encoded sequence
            flag = runlength & 0x80;
            if ( flag )
            {
                runlength -= 128;
            }
            skip = 0;
        }

        // do we need to skip reading this pixel?
        if ( !skip )
        {
            // no, read in the pixel data
            if((step + sizeof(unsigned char) * mode) > bufSize)
            {
                break;
            }
            memcpy(aux, buffer + step, sizeof(unsigned char) * mode);
            step += sizeof(unsigned char) * mode;

            // mode=3 or 4 implies that the image is RGB(A). However TGA
            // stores it as BGR(A) so we'll have to swap R and B.
            if ( mode >= 3 )
            {
                unsigned char tmp;

                tmp = aux[0];
                aux[0] = aux[2];
                aux[2] = tmp;
            }
        }

        // add the pixel to our image
        memcpy(&info->imageData[index], aux, mode);
        index += mode;
    }

    return true;
}

void CTgaLib::TgaFlipImage( TImageTGA *info )
{
    // mode equal the number of components for each pixel
    int mode = info->pixelDepth / 8;
    int rowbytes = info->width*mode;
    unsigned char *row = new unsigned char[rowbytes];
    int y;

    if (row != NULL)
    {
        for( y = 0; y < (info->height/2); y++ )
        {
            memcpy(row, &info->imageData[y*rowbytes],rowbytes);
            memcpy(&info->imageData[y*rowbytes], &info->imageData[(info->height-(y+1))*rowbytes], rowbytes);
            memcpy(&info->imageData[(info->height-(y+1))*rowbytes], row, rowbytes);
        }

        BEATS_SAFE_DELETE_ARRAY( row );
        info->flipped = 0;
    }
}

SImageTGA * CTgaLib::TgaLoad( const TCHAR* pszFileName )
{
    SImageTGA * pReturn = NULL;
    CSerializer serializer(pszFileName);

    if (serializer.GetWritePos() != 0)
    {
        pReturn = TgaLoadBuffer(serializer.GetBuffer(), serializer.GetWritePos());
    }

    return pReturn;
}

void CTgaLib::TgaRGBtogreyscale( SImageTGA *info )
{
    int mode,i,j;

    unsigned char *newImageData = NULL;

    // if the image is already grayscale do nothing
    if (info->pixelDepth == 8)
        return;

    // compute the number of actual components
    mode = info->pixelDepth / 8;

    // allocate an array for the new image data
    newImageData = new unsigned char[ info->height * info->width ];
    if (newImageData == NULL)
    {
        return;
    }

    // convert pixels: grayscale = o.30 * R + 0.59 * G + 0.11 * B
    for (i = 0,j = 0; j < info->width * info->height; i +=mode, j++)
        newImageData[j] =    
        (unsigned char)(0.30 * info->imageData[i] +
        0.59 * info->imageData[i+1] +
        0.11 * info->imageData[i+2]);


    //free old image data
    BEATS_SAFE_DELETE_ARRAY( info->imageData );

    // reassign pixelDepth and type according to the new image type
    info->pixelDepth = 8;
    info->type = 3;
    // reassigning imageData to the new array.
    info->imageData = newImageData;
}

void CTgaLib::TgaDestroy( SImageTGA *info )
{
    BEATS_SAFE_DELETE( info );
}

