#include "stdafx.h"
#include "AudioPlayer.h"
#include "AudioCache.h"
#include "mpg123.h"
#include "vorbis/vorbisfile.h"
#include "EnginePublic/PublicDef.h"
#include "Audio/include/AudioEngine.h"

CAudioPlayer::CAudioPlayer()
: m_bExitThread(false)
, m_bStreamingSource(false)
, m_bReady(false)
, m_pAudioCache(nullptr)
, m_fVolume(1.0f)
, m_bLoop(false)
, m_bReadForRemove(false)
{

}

CAudioPlayer::CAudioPlayer(const CAudioPlayer& player)
{
    m_bExitThread = player.m_bExitThread;
    m_bStreamingSource = player.m_bStreamingSource;
    m_bReady = player.m_bReady;
    m_pAudioCache = player.m_pAudioCache;
    m_bReadForRemove = player.m_bReadForRemove;
    m_bLoop = player.m_bLoop;
    m_fVolume = player.m_fVolume;
}

CAudioPlayer::~CAudioPlayer()
{
    m_bExitThread = true;
    if (m_bStreamingSource)
    {
        m_sleepCondition.notify_all();
        if (m_rotateBufferThread.joinable())
        {
            m_rotateBufferThread.join();
        }
        alDeleteBuffers(QUEUEBUFFER_NUM, m_bufferIds);
    }
}

void CAudioPlayer::NotifyExitThread()
{
    if (m_pAudioCache && m_pAudioCache->m_nQueBufferFrames > 0)
    {
        std::unique_lock<std::mutex> lk(m_sleepMutex);
        m_bExitThread = true;
        m_sleepCondition.notify_all();
    }
}

bool CAudioPlayer::Play2d(CAudioCache* cache)
{
    bool bRet = false;
    if (cache->m_bAlBufferReady)
    {
        m_pAudioCache = cache;

        alSourcei(m_alSource, AL_BUFFER, NULL);
        alSourcef(m_alSource, AL_PITCH, 1.0f);
        alSourcef(m_alSource, AL_GAIN, m_fVolume);
        bool bStatic = m_pAudioCache->m_nQueBufferFrames == 0;
        alSourcei(m_alSource, AL_LOOPING, bStatic ? m_bLoop ? AL_TRUE : AL_FALSE : AL_FALSE);
        if (bStatic)
        {
            alSourcei(m_alSource, AL_BUFFER, m_pAudioCache->m_alBufferId);
            bRet = true;
        }
        else
        {
            auto alError = alGetError();
            alGenBuffers(QUEUEBUFFER_NUM, m_bufferIds);
            alError = alGetError();
            if (alError == AL_NO_ERROR)
            {
                for (int index = 0; index < QUEUEBUFFER_NUM; ++index)
                {
                    alBufferData(m_bufferIds[index], m_pAudioCache->m_alBufferFormat, m_pAudioCache->m_queBuffers[index], m_pAudioCache->m_queBufferSize[index], m_pAudioCache->m_sampleRate);
                }
                alSourceQueueBuffers(m_alSource, QUEUEBUFFER_NUM, m_bufferIds);
                bRet = true;
                m_bStreamingSource = true;
                m_rotateBufferThread = std::thread(&CAudioPlayer::RotateBufferThread, this, m_pAudioCache->m_nQueBufferFrames * QUEUEBUFFER_NUM + 1);
            }
            else
            {
                printf("%s:alGenBuffers error code:%x\n", __FUNCTION__, alError);
            }
        }
        if (bRet)
        {
            alSourcePlay(m_alSource);
            m_bReady = true;
            auto alError = alGetError();
            bRet = alError == AL_NO_ERROR;
            if (!bRet)
            {
                printf("%s:alSourcePlay error code:%x\n", __FUNCTION__, alError);
            }
        }
    }
    return bRet;
}

void CAudioPlayer::RotateBufferThread(int offsetFrame)
{
    ALint sourceState;
    ALint bufferProcessed = 0;
    mpg123_handle* mpg123handle = nullptr;
    OggVorbis_File* vorbisFile = nullptr;

    auto audioFileFormat = m_pAudioCache->m_eFileFormat;
    char* tmpBuffer = (char*)malloc(m_pAudioCache->m_nQueBufferBytes);

    switch (audioFileFormat)
    {
        case CAudioCache::EFileFormat::MP3:
        {
            int error = MPG123_OK;
            mpg123handle = mpg123_new(nullptr, &error);
            if (!mpg123handle)
            {
                BEATS_ASSERT(false, "Basic setup goes wrong: %s", mpg123_plain_strerror(error));
                goto ExitBufferThread;
            }
            bool bOk = false;
            long rate = 0;
            int channels = 0;
            int mp3Encoding = 0;
            std::string strPath = m_pAudioCache->m_strFileFullPath;
            if (mpg123_open(mpg123handle, strPath.c_str()) == MPG123_OK)
            {
                if (mpg123_getformat(mpg123handle, &rate, &channels, &mp3Encoding) == MPG123_OK)
                {
                    /* Ensure that this output format will not change (it could, when we allow it). */
                    mpg123_format_none(mpg123handle);
                    mpg123_format(mpg123handle, m_pAudioCache->m_sampleRate, m_pAudioCache->m_nChannels, m_pAudioCache->m_nMp3Encoding);

                    if (offsetFrame != 0)
                    {
                        mpg123_seek(mpg123handle, offsetFrame, SEEK_SET);
                    }
                    bOk = true;
                }
            }
            
            if (!bOk)
            {
                BEATS_ASSERT(false, "Trouble with mpg123: %s\n", mpg123_strerror(mpg123handle));
                goto ExitBufferThread;
            }

            break;
        }
        case CAudioCache::EFileFormat::OGG:
        {
            vorbisFile = new OggVorbis_File;
            if (ov_fopen(m_pAudioCache->m_strFileFullPath.c_str(), vorbisFile))
            {
                BEATS_ASSERT(false, "Input does not appear to be an Ogg bitstream.\n");
                goto ExitBufferThread;
            }
            if (offsetFrame != 0)
            {
                ov_pcm_seek(vorbisFile, offsetFrame);
            }
            break;
        }
        default:
            break;
    }
    alSourcePlay(m_alSource);
    while (!m_bExitThread)
    {
        alGetSourcei(m_alSource, AL_SOURCE_STATE, &sourceState);
        if (sourceState == AL_PLAYING)
        {
            m_bReady = true;
            alGetSourcei(m_alSource, AL_BUFFERS_PROCESSED, &bufferProcessed);
            while (bufferProcessed > 0)
            {
                bufferProcessed--;
                size_t readRet = 0;
                if (audioFileFormat == CAudioCache::EFileFormat::MP3)
                {
                    mpg123_read(mpg123handle, (unsigned char*)tmpBuffer, m_pAudioCache->m_nQueBufferBytes, &readRet);
                    if (readRet <= 0)
                    {
                        if (m_bLoop)
                        {
                            mpg123_seek(mpg123handle, 0, SEEK_SET);
                            mpg123_read(mpg123handle, (unsigned char*)tmpBuffer, m_pAudioCache->m_nQueBufferBytes, &readRet);
                        }
                        else
                        {
                            m_bExitThread = true;
                            break;
                        }
                    }
                    else
                    {
                        m_pAudioCache->m_uBytesOfRead += readRet;
                    }
                }
                else if (audioFileFormat == CAudioCache::EFileFormat::OGG)
                {
                    int current_section;
                    readRet = ov_read(vorbisFile, tmpBuffer, m_pAudioCache->m_nQueBufferBytes, 0, 2, 1, &current_section);
                    if (readRet <= 0)
                    {
                        if (m_bLoop)
                        {
                            ov_pcm_seek(vorbisFile, 0);
                            readRet = ov_read(vorbisFile, tmpBuffer, m_pAudioCache->m_nQueBufferBytes, 0, 2, 1, &current_section);
                        }
                        else
                        {
                            m_bExitThread = true;
                            break;
                        }
                    }
                }

                ALuint bid;
                alSourceUnqueueBuffers(m_alSource, 1, &bid);
                alBufferData(bid, m_pAudioCache->m_alBufferFormat, tmpBuffer, readRet, m_pAudioCache->m_sampleRate);
                alSourceQueueBuffers(m_alSource, 1, &bid);
            }
        }

        std::unique_lock<std::mutex> lk(m_sleepMutex);
        if (m_bExitThread)
        {
            break;
        }
        m_sleepCondition.wait_for(lk, std::chrono::milliseconds(35));
    }
ExitBufferThread:
    switch (audioFileFormat)
    {
    case CAudioCache::EFileFormat::OGG:
        ov_clear(vorbisFile);
        delete vorbisFile;
        break;
    case CAudioCache::EFileFormat::MP3:
        mpg123_close(mpg123handle);
        mpg123_delete(mpg123handle);
        break;
    case CAudioCache::EFileFormat::UNKNOWN:
    default:
        break;
    }
    free(tmpBuffer);
    m_bReadForRemove = true;
}

bool CAudioPlayer::SetLoop(bool loop)
{
    bool bRet = !m_bExitThread;
    if (bRet)
    {
        m_bLoop = loop;
    }
    return bRet;
}

