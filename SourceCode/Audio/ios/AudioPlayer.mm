#import <Foundation/Foundation.h>

#include "AudioPlayer.h"
#include "AudioCache.h"
#include "EnginePublic/BeyondEnginePublic.h"
#import <AudioToolbox/ExtendedAudioFile.h>

CAudioPlayer::CAudioPlayer()
: m_pAudioCache(nullptr)
, m_bReady(false)
, m_fCurrTime(0.0f)
, m_bStreamingSource(false)
, m_bExitThread(false)
, m_bReadForRemove(false)
{
}

CAudioPlayer::~CAudioPlayer()
{
    m_bExitThread = true;
    alSourcei(m_alSource, AL_BUFFER, 0);
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
    if (m_pAudioCache && m_pAudioCache->m_uQueBufferFrames > 0)
    {
        std::unique_lock<std::mutex> lk(m_sleepMutex);
        m_bExitThread = true;
        m_sleepCondition.notify_all();
    }
}

bool CAudioPlayer::Play2d(CAudioCache* cache)
{
    bool bRet = false;
    BEATS_ASSERT(cache->m_bAlBufferReady);
    auto alError = alGetError();
    if (cache->m_bAlBufferReady)
    {
        m_pAudioCache = cache;

        alSourcei(m_alSource, AL_BUFFER, 0);
        alSourcef(m_alSource, AL_PITCH, 1.0f);
        alSourcef(m_alSource, AL_GAIN, m_fVolume);
        bool bStatic = m_pAudioCache->m_uQueBufferFrames == 0;
        alSourcei(m_alSource, AL_LOOPING, bStatic ? m_bLoop ? AL_TRUE : AL_FALSE : AL_FALSE);
        alError = alGetError();
        BEATS_ASSERT(alError == AL_NO_ERROR, "%s:set params error code:%x\n", __PRETTY_FUNCTION__, alError);
        if (bStatic)
        {
            alSourcei(m_alSource, AL_BUFFER, m_pAudioCache->m_alBufferId);
            alError = alGetError();
            BEATS_ASSERT(alError == AL_NO_ERROR, "%s:bind buffer error code:%x\n", __PRETTY_FUNCTION__, alError);
            bRet = true;
        }
        else
        {
            alGenBuffers(QUEUEBUFFER_NUM, m_bufferIds);
            alError = alGetError();
            if (alError == AL_NO_ERROR)
            {
                for (int index = 0; index < QUEUEBUFFER_NUM; ++index)
                {
                    alBufferData(m_bufferIds[index], m_pAudioCache->m_format, m_pAudioCache->m_queBuffers[index], m_pAudioCache->m_queBufferSize[index], m_pAudioCache->m_sampleRate);
                    alError = alGetError();
                    BEATS_ASSERT(alError == AL_NO_ERROR, "%s:buffer data error code:%x\n", __PRETTY_FUNCTION__, alError);
                }
                alSourceQueueBuffers(m_alSource, QUEUEBUFFER_NUM, m_bufferIds);
                alError = alGetError();
                BEATS_ASSERT(alError == AL_NO_ERROR, "%s:alSourceQueueBuffers error code:%x\n", __PRETTY_FUNCTION__, alError);
                bRet = true;
                m_bStreamingSource = true;
                m_rotateBufferThread = std::thread(&CAudioPlayer::RotateBufferThread, this, m_pAudioCache->m_uQueBufferFrames * QUEUEBUFFER_NUM + 1);
                BEATS_PRINT("%s: use QueueBuffers\n", __PRETTY_FUNCTION__);
            }
            else
            {
                BEATS_PRINT("%s:alGenBuffers error code:%x\n", __PRETTY_FUNCTION__, alError);
            }
        }
        if (bRet)
        {
            alSourcePlay(m_alSource);
            m_bReady = true;
            alError = alGetError();
            bRet = alError == AL_NO_ERROR;
            if (!bRet)
            {
                BEATS_PRINT("%s:alSourcePlay error code:%x\n", __PRETTY_FUNCTION__, alError);
            }
        }
    }
    else
    {
        BEATS_PRINT("error buffer not ready!\n");
    }
    return bRet;
}

void CAudioPlayer::RotateBufferThread(int offsetFrame)
{
    ALint sourceState;
    ALint bufferProcessed = 0;
    ExtAudioFileRef extRef = nullptr;

    NSString *fileFullPath = [[NSString alloc] initWithCString:m_pAudioCache->m_strFileFullPath.c_str() encoding : [NSString defaultCStringEncoding]];
    auto fileURL = (CFURLRef)[[NSURL alloc] initFileURLWithPath:fileFullPath];
    [fileFullPath release];
    char* tmpBuffer = (char*)malloc(m_pAudioCache->m_uQueBufferBytes);
    auto frames = m_pAudioCache->m_uQueBufferFrames;

    auto error = ExtAudioFileOpenURL(fileURL, &extRef);
    if (error)
    {
        BEATS_PRINT("%s: ExtAudioFileOpenURL FAILED, Error = %ld\n", __PRETTY_FUNCTION__, (long)error);
        goto ExitBufferThread;
    }

    error = ExtAudioFileSetProperty(extRef, kExtAudioFileProperty_ClientDataFormat, sizeof(m_pAudioCache->m_outputFormat), &m_pAudioCache->m_outputFormat);
    AudioBufferList theDataBuffer;
    theDataBuffer.mNumberBuffers = 1;
    theDataBuffer.mBuffers[0].mData = tmpBuffer;
    theDataBuffer.mBuffers[0].mDataByteSize = m_pAudioCache->m_uQueBufferBytes;
    theDataBuffer.mBuffers[0].mNumberChannels = m_pAudioCache->m_outputFormat.mChannelsPerFrame;

    if (offsetFrame != 0)
    {
        ExtAudioFileSeek(extRef, offsetFrame);
    }
    while (!m_bExitThread)
    {
        alGetSourcei(m_alSource, AL_SOURCE_STATE, &sourceState);
        if (sourceState == AL_PLAYING)
        {
            alGetSourcei(m_alSource, AL_BUFFERS_PROCESSED, &bufferProcessed);
            while (bufferProcessed > 0)
            {
                bufferProcessed--;

                frames = m_pAudioCache->m_uQueBufferFrames;
                ExtAudioFileRead(extRef, (UInt32*)&frames, &theDataBuffer);
                if (frames <= 0)
                {
                    if (m_bLoop)
                    {
                        ExtAudioFileSeek(extRef, 0);
                        frames = m_pAudioCache->m_uQueBufferFrames;
                        theDataBuffer.mBuffers[0].mDataByteSize = m_pAudioCache->m_uQueBufferBytes;
                        ExtAudioFileRead(extRef, (UInt32*)&frames, &theDataBuffer);
                    }
                    else
                    {
                        m_bExitThread = true;
                        break;
                    }
                }

                ALuint bid;
                alSourceUnqueueBuffers(m_alSource, 1, &bid);
                alBufferData(bid, m_pAudioCache->m_format, tmpBuffer, frames * m_pAudioCache->m_outputFormat.mBytesPerFrame, m_pAudioCache->m_sampleRate);
                alSourceQueueBuffers(m_alSource, 1, &bid);
            }
        }

        if (m_bExitThread)
        {
            break;
        }
        std::unique_lock<std::mutex> lk(m_sleepMutex);
        m_sleepCondition.wait_for(lk, std::chrono::milliseconds(75));
    }

ExitBufferThread:
    CFRelease(fileURL);
    // Dispose the ExtAudioFileRef, it is no longer needed
    if (extRef)
    {
        ExtAudioFileDispose(extRef);
    }
    free(tmpBuffer);
    m_bReadForRemove = true;
}

bool CAudioPlayer::SetLoop(bool loop)
{
    if (!m_bExitThread)
    {
        m_bLoop = loop;
    }

    return !m_bExitThread;
}
