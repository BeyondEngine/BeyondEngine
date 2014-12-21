#include "stdafx.h"
#include "AudioCache.h"
#include "vorbis/vorbisfile.h"
#include "vorbis/codec.h"
#include "mpg123.h"
#include "Utility/BeatsUtility/FilePathTool.h"

#define PCMDATA_CACHEMAXSIZE 2621440

CAudioCache::CAudioCache()
: m_pPcmData(nullptr)
, m_uPcmDataSize(0)
, m_uBytesOfRead(0)
, m_bAlBufferReady(false)
, m_bLoadFail(false)
, m_eFileFormat(EFileFormat::UNKNOWN)
, m_nQueBufferFrames(0)
, m_nQueBufferBytes(0)
, m_nMp3Encoding(0)
, m_nChannels(0)
, m_fDuration(0.0f)
, m_uBytesPerFrame(0)
{
    memset(m_queBuffers, 0, sizeof(m_queBuffers));
    memset(m_queBufferSize, 0, sizeof(m_queBufferSize));
}

CAudioCache::CAudioCache(const CAudioCache& cache)
{
    m_pPcmData = cache.m_pPcmData;
    m_uPcmDataSize = cache.m_uPcmDataSize;
    m_uBytesOfRead = cache.m_uBytesOfRead;
    m_bAlBufferReady = cache.m_bAlBufferReady;
    m_eFileFormat = cache.m_eFileFormat;
    m_nQueBufferFrames = cache.m_nQueBufferFrames;
    m_nQueBufferBytes = cache.m_nQueBufferBytes;
    m_nMp3Encoding = cache.m_nMp3Encoding;
    m_bLoadFail = cache.m_bLoadFail;
    m_nChannels = cache.m_nChannels;
    m_fDuration = cache.m_fDuration;
    m_uBytesPerFrame = cache.m_uBytesPerFrame;
}

CAudioCache::~CAudioCache()
{
    if (m_pPcmData){
        if (m_bAlBufferReady){
            alDeleteBuffers(1, &m_alBufferId);
        }
        //wait for the 'readDataTask' task to exit
        m_readDataTaskMutex.lock();
        m_readDataTaskMutex.unlock();

        free(m_pPcmData);
    }

    if (m_nQueBufferFrames > 0) {
        for (int index = 0; index < QUEUEBUFFER_NUM; ++index) {
            free(m_queBuffers[index]);
        }
    }
}

void CAudioCache::ReadDataTask()
{
    m_readDataTaskMutex.lock();

    OggVorbis_File* vf = nullptr;
    mpg123_handle* mpg123handle = nullptr;
    int32_t totalFrames = 0;
    BEATS_ASSERT(CFilePathTool::GetInstance()->Exists(m_strFileFullPath.c_str()));
    switch (m_eFileFormat)
    {
    case EFileFormat::OGG:
    {
        vf = new OggVorbis_File;
        if (ov_fopen(m_strFileFullPath.c_str(), vf)){
            BEATS_ASSERT(false, "Input does not appear to be an Ogg bitstream.\n");
            goto ExitThread;
        }

        auto vi = ov_info(vf, -1);
        totalFrames = (int32_t)ov_pcm_total(vf, -1);
        m_uBytesPerFrame = vi->channels * 2;
        m_alBufferFormat = (vi->channels > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
        m_sampleRate = vi->rate;
        m_uPcmDataSize = totalFrames * m_uBytesPerFrame;
        m_fDuration = 1.0f * totalFrames / m_sampleRate;
    }
        break;
    case EFileFormat::MP3:
    {
        long rate = 0;
        int error = MPG123_OK;
        mpg123handle = mpg123_new(nullptr, &error);
        if (!mpg123handle){
            BEATS_ASSERT(false, "Basic setup goes wrong: %s", mpg123_plain_strerror(error));
            goto ExitThread;
        }

        if (mpg123_open(mpg123handle, m_strFileFullPath.c_str()) != MPG123_OK ||
            mpg123_getformat(mpg123handle, &rate, &m_nChannels, &m_nMp3Encoding) != MPG123_OK) {
            BEATS_ASSERT(false, "Trouble with mpg123: %s\n", mpg123_strerror(mpg123handle));
            goto ExitThread;
        }

        if (m_nMp3Encoding == MPG123_ENC_SIGNED_16){
            m_uBytesPerFrame = 2 * m_nChannels;
        }
        else if (m_nMp3Encoding == MPG123_ENC_FLOAT_32){
            m_uBytesPerFrame = 4 * m_nChannels;
        }
        else{
            BEATS_ASSERT(false, "Bad encoding: 0x%x!\n", m_nMp3Encoding);
            goto ExitThread;
        }

        m_alBufferFormat = (m_nChannels > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
        m_sampleRate = rate;

        /* Ensure that this output format will not change (it could, when we allow it). */
        mpg123_format_none(mpg123handle);
        mpg123_format(mpg123handle, rate, m_nChannels, m_nMp3Encoding);
        /* Ensure that we can get accurate length by call mpg123_length */
        mpg123_scan(mpg123handle);

        auto framesLength = mpg123_length(mpg123handle);
        totalFrames = framesLength;
        m_uPcmDataSize = totalFrames * m_uBytesPerFrame;
        m_fDuration = 1.0f * totalFrames / m_sampleRate;
    }
        break;
    case EFileFormat::UNKNOWN:
    default:
        break;
    }

    if (m_uPcmDataSize <= PCMDATA_CACHEMAXSIZE)
    {
        m_pPcmData = malloc(m_uPcmDataSize);
        auto alError = alGetError();
        alGenBuffers(1, &m_alBufferId);
        alError = alGetError();
        if (alError != AL_NO_ERROR) {
            BEATS_ASSERT(false, "%s: attaching audio to buffer fail: %x\n", __FUNCTION__, alError);
            goto ExitThread;
        }

        switch (m_eFileFormat)
        {
        case EFileFormat::OGG:
        {
            int current_section;
            int32_t readRet = 0;
            do
            {
                readRet = ov_read(vf, (char*)m_pPcmData + m_uBytesOfRead, 4096, 0, 2, 1, &current_section);
                if (readRet > 0){
                    m_uBytesOfRead += readRet;
                }
            } while (m_uBytesOfRead < m_uPcmDataSize);

            m_bAlBufferReady = true;
            m_uBytesOfRead = m_uPcmDataSize;
            break;
        }
        case EFileFormat::MP3:
        {
            size_t done = 0;
            auto err = mpg123_read(mpg123handle, (unsigned char*)m_pPcmData, m_uPcmDataSize, &done);
            if (err == MPG123_ERR){
                BEATS_ASSERT(false, "Trouble with mpg123: %s\n", mpg123_strerror(mpg123handle));
                goto ExitThread;
            }
            if (err == MPG123_DONE || err == MPG123_OK){
                m_bAlBufferReady = true;
                m_uPcmDataSize = done;
                m_uBytesOfRead = done;
            }
        }
            break;
        case EFileFormat::UNKNOWN:
        default:
            break;
        }
        alBufferData(m_alBufferId, m_alBufferFormat, m_pPcmData, m_uPcmDataSize, m_sampleRate);
    }
    else
    {
        m_nQueBufferFrames = (int)(m_sampleRate * QUEUEBUFFER_TIME_STEP);
        m_nQueBufferBytes = m_nQueBufferFrames * m_uBytesPerFrame;

        for (int index = 0; index < QUEUEBUFFER_NUM; ++index) {
            m_queBuffers[index] = (char*)malloc(m_nQueBufferBytes);

            switch (m_eFileFormat){
            case EFileFormat::MP3:
            {
                size_t done = 0;
                auto err = mpg123_read(mpg123handle, (unsigned char*)m_queBuffers[index], m_nQueBufferBytes, &done);
                if (err == MPG123_ERR)
                {
                    BEATS_ASSERT(false, "Trouble with mpg123: %s\n", mpg123_strerror(mpg123handle));
                    goto ExitThread;
                }
                m_queBufferSize[index] = done;
                m_uBytesOfRead += done;
            }
                break;
            case EFileFormat::OGG:
            {
                int current_section;
                auto readRet = ov_read(vf, m_queBuffers[index], m_nQueBufferBytes, 0, 2, 1, &current_section);
                m_queBufferSize[index] = readRet;
            }
                break;
            }
        }
        m_bAlBufferReady = true;
    }

ExitThread:
    switch (m_eFileFormat)
    {
    case EFileFormat::OGG:
        ov_clear(vf);
        delete vf;
        break;
    case EFileFormat::MP3:
        mpg123_close(mpg123handle);
        mpg123_delete(mpg123handle);
        break;
    case EFileFormat::UNKNOWN:
    default:
        break;
    }

    m_readDataTaskMutex.unlock();
    m_bLoadFail = !m_bAlBufferReady;

    InvokingPlayCallbacks();
}

void CAudioCache::InvokingPlayCallbacks()
{
    m_callbackMutex.lock();
    auto count = m_callbacks.size();
    for (size_t index = 0; index < count; ++index)
    {
        m_callbacks[index]();
    }
    m_callbacks.clear();
    m_callbackMutex.unlock();
}

void CAudioCache::AddPlayCallback(const std::function<void()> &callback)
{
    m_callbacks.push_back(callback);
}
