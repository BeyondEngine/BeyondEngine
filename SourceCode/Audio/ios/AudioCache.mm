#include "AudioCache.h"
#include "stdafx.h"
#include "vorbis/vorbisfile.h"
#include "vorbis/codec.h"
#include "Utility/BeatsUtility/FilePathTool.h"

#import <Foundation/Foundation.h>
#import <OpenAL/alc.h>
#import <AudioToolbox/ExtendedAudioFile.h>
#include <thread>

#define PCMDATA_CACHEMAXSIZE 1048576

typedef ALvoid AL_APIENTRY (*alBufferDataStaticProcPtr) (const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
static ALvoid alBufferDataStaticProc(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
{
    static alBufferDataStaticProcPtr proc = NULL;
    
    if (proc == NULL)
    {
        proc = (alBufferDataStaticProcPtr) alcGetProcAddress(NULL, (const ALCchar*) "alBufferDataStatic");
    }
    
    if (proc)
    {
        proc(bid, format, data, size, freq);
    }
}
CAudioCache::CAudioCache()
: m_dataSize(0)
, m_pPcmData(nullptr)
, m_bytesOfRead(0)
, m_uQueBufferFrames(0)
, m_uQueBufferBytes(0)
, m_bAlBufferReady(false)
, m_bLoadFail(false)
, m_bExitReadDataTask(false)
{
}

CAudioCache::~CAudioCache()
{
    m_bExitReadDataTask = true;
    if(m_pPcmData)
    {
        if (m_bAlBufferReady)
        {
            alDeleteBuffers(1, &m_alBufferId);
        }
        //wait for the 'ReadDataTask' task to exit
        m_readDataTaskMutex.lock();
        m_readDataTaskMutex.unlock();
        
        free(m_pPcmData);
    }
    
    if (m_uQueBufferFrames > 0)
    {
        for (int index = 0; index < QUEUEBUFFER_NUM; ++index)
        {
            free(m_queBuffers[index]);
        }
    }
}

void CAudioCache::ReadOgg()
{
    auto bRet = CFilePathTool::GetInstance()->Exists(m_strFileFullPath.c_str());
    if (bRet)
    {
        m_readDataTaskMutex.lock();
        
        OggVorbis_File* vf = nullptr;
        vorbis_info* vi = nullptr;
        long totalFrames = 0;
        int nChannels = 0;
        
        vf = new OggVorbis_File;
        auto error = ov_fopen(m_strFileFullPath.c_str(), vf);
        if (error)
        {
            BEATS_ASSERT(false, "Input does not appear to be an Ogg bitstream.\n");
            goto ExitThread;
        }
        
        //file read
        vi = ov_info(vf, -1);
        nChannels = vi->channels;       //keep channels
        totalFrames = (long)ov_pcm_total(vf, -1);
        m_nBytesPerFrame = vi->channels * 2;
        m_format = (vi->channels > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
        m_sampleRate = vi->rate;
        m_dataSize = totalFrames * m_nBytesPerFrame;
        m_fDuration = 1.0f * totalFrames / m_sampleRate;
        
        //stream decode
        if (m_dataSize <= PCMDATA_CACHEMAXSIZE)
        {
            m_pPcmData = (char*)malloc(m_dataSize);
            auto alError = alGetError();
            alGenBuffers(1, &m_alBufferId);
            alError = alGetError();
            if (alError != AL_NO_ERROR)
            {
                BEATS_ASSERT(false, "%s: attaching audio to buffer fail: %x\n", __FUNCTION__, alError);
                goto ExitThread;
            }
            
            int current_section;
            long readRet = 0;
            do
            {
                readRet = ov_read(vf, (char*)m_pPcmData + m_bytesOfRead, 4096, 0, 2, 1, &current_section);
                if (readRet > 0){
                    m_bytesOfRead += readRet;
                }
            } while (m_bytesOfRead < m_dataSize);
            
            m_bAlBufferReady = true;
            m_bytesOfRead = m_dataSize;
            
            alBufferData(m_alBufferId, m_format, m_pPcmData, m_dataSize, m_sampleRate);
        }
        else
        {
            m_uQueBufferFrames = (int)(m_sampleRate * QUEUEBUFFER_TIME_STEP);
            m_uQueBufferBytes = m_uQueBufferFrames * m_nBytesPerFrame;
            for (int index = 0; index < QUEUEBUFFER_NUM; ++index)
            {
                m_queBuffers[index] = (char*)malloc(m_uQueBufferBytes);
                int current_section;
                auto readRet = ov_read(vf, m_queBuffers[index], m_uQueBufferBytes, 0, 2, 1, &current_section);
                m_queBufferSize[index] = readRet;
            }
            m_bAlBufferReady = true;
        }
        
        // Set the client format to 16 bit signed integer (native-endian) data
        // Maintain the channel count and sample rate of the original source format
        m_outputFormat.mSampleRate = m_sampleRate;
        m_outputFormat.mChannelsPerFrame = nChannels;
        
        m_nBytesPerFrame = 2 * m_outputFormat.mChannelsPerFrame;
        m_outputFormat.mFormatID = kAudioFormatLinearPCM;
        m_outputFormat.mBytesPerPacket = m_nBytesPerFrame;
        m_outputFormat.mFramesPerPacket = 1;
        m_outputFormat.mBytesPerFrame = m_nBytesPerFrame;
        m_outputFormat.mBitsPerChannel = 16;
        m_outputFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
        
    ExitThread:
        ov_clear(vf);
        delete vf;
        m_readDataTaskMutex.unlock();
        if(!m_bAlBufferReady)
        {
            m_bAlBufferReady = m_uQueBufferFrames > 0;
        }
        m_bLoadFail = !m_bAlBufferReady;
        InvokingPlayCallbacks();
    }
    else
    {
        BEATS_ASSERT(false,"can not find sound file %s", m_strFileFullPath.c_str());
    }
}

void CAudioCache::ReadOthers()
{
    m_readDataTaskMutex.lock();
    
    AudioStreamBasicDescription theFileFormat;
    UInt32 thePropertySize = sizeof(theFileFormat);
    
    SInt64 theFileLengthInFrames;
    SInt64 readInFrames;
    SInt64 dataSize;
    SInt64 frames;
    AudioBufferList theDataBuffer;
    ExtAudioFileRef extRef = nullptr;
    
    NSString *fileFullPath = [[NSString alloc] initWithCString:m_strFileFullPath.c_str() encoding:[NSString defaultCStringEncoding]];
    auto fileURL = (CFURLRef)[[NSURL alloc] initFileURLWithPath:fileFullPath];
    [fileFullPath release];
    
    auto error = ExtAudioFileOpenURL(fileURL, &extRef);
    if(error)
    {
        printf("%s: ExtAudioFileOpenURL %s FAILED, Error = %ld\n", __PRETTY_FUNCTION__, m_strFileFullPath.c_str(), (long)error);
        goto ExitThread;
    }
    
    // Get the audio data format
    error = ExtAudioFileGetProperty(extRef, kExtAudioFileProperty_FileDataFormat, &thePropertySize, &theFileFormat);
    if(error)
    {
        printf("%s: ExtAudioFileGetProperty(kExtAudioFileProperty_FileDataFormat) FAILED, Error = %ld\n", __PRETTY_FUNCTION__, (long)error);
        goto ExitThread;
    }
    if (theFileFormat.mChannelsPerFrame > 2)
    {
        printf("%s: Unsupported Format, channel count is greater than stereo\n",__PRETTY_FUNCTION__);
        goto ExitThread;
    }
    
    // Set the client format to 16 bit signed integer (native-endian) data
    // Maintain the channel count and sample rate of the original source format
    m_outputFormat.mSampleRate = theFileFormat.mSampleRate;
    m_outputFormat.mChannelsPerFrame = theFileFormat.mChannelsPerFrame;
    
    m_nBytesPerFrame = 2 * m_outputFormat.mChannelsPerFrame;
    m_outputFormat.mFormatID = kAudioFormatLinearPCM;
    m_outputFormat.mBytesPerPacket = m_nBytesPerFrame;
    m_outputFormat.mFramesPerPacket = 1;
    m_outputFormat.mBytesPerFrame = m_nBytesPerFrame;
    m_outputFormat.mBitsPerChannel = 16;
    m_outputFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
    
    error = ExtAudioFileSetProperty(extRef, kExtAudioFileProperty_ClientDataFormat, sizeof(m_outputFormat), &m_outputFormat);
    if(error)
    {
        printf("%s: ExtAudioFileSetProperty FAILED, Error = %ld\n", __PRETTY_FUNCTION__, (long)error);
        goto ExitThread;
    }
    
    // Get the total frame count
    thePropertySize = sizeof(theFileLengthInFrames);
    error = ExtAudioFileGetProperty(extRef, kExtAudioFileProperty_FileLengthFrames, &thePropertySize, &theFileLengthInFrames);
    if(error)
    {
        printf("%s: ExtAudioFileGetProperty(kExtAudioFileProperty_FileLengthFrames) FAILED, Error = %ld\n", __PRETTY_FUNCTION__, (long)error);
        goto ExitThread;
    }
    
    m_dataSize = (ALsizei)(theFileLengthInFrames * m_outputFormat.mBytesPerFrame);
    m_format = (m_outputFormat.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
    m_sampleRate = (ALsizei)m_outputFormat.mSampleRate;
    m_fDuration = 1.0f * theFileLengthInFrames / m_outputFormat.mSampleRate;
    
    if (m_dataSize <= PCMDATA_CACHEMAXSIZE)
    {
        m_pPcmData = (char*)malloc(m_dataSize);
        alGenBuffers(1, &m_alBufferId);
        auto alError = alGetError();
        if (alError != AL_NO_ERROR)
        {
            printf("%s: attaching audio to buffer fail: %x\n", __PRETTY_FUNCTION__, alError);
            goto ExitThread;
        }
        alBufferDataStaticProc(m_alBufferId, m_format, m_pPcmData, m_dataSize, m_sampleRate);
        
        readInFrames = theFileFormat.mSampleRate * QUEUEBUFFER_TIME_STEP * QUEUEBUFFER_NUM;
        dataSize = m_outputFormat.mBytesPerFrame * readInFrames;
        if (dataSize > m_dataSize)
        {
            dataSize = m_dataSize;
            readInFrames = theFileLengthInFrames;
        }
        theDataBuffer.mNumberBuffers = 1;
        theDataBuffer.mBuffers[0].mDataByteSize = (UInt32)dataSize;
        theDataBuffer.mBuffers[0].mNumberChannels = m_outputFormat.mChannelsPerFrame;
        
        theDataBuffer.mBuffers[0].mData = m_pPcmData;
        frames = readInFrames;
        ExtAudioFileRead(extRef, (UInt32*)&frames, &theDataBuffer);
        m_bAlBufferReady = true;
        m_bytesOfRead += dataSize;
        InvokingPlayCallbacks();
        
        while (!m_bExitReadDataTask && m_bytesOfRead + dataSize < m_dataSize)
        {
            theDataBuffer.mBuffers[0].mData = m_pPcmData + m_bytesOfRead;
            frames = readInFrames;
            ExtAudioFileRead(extRef, (UInt32*)&frames, &theDataBuffer);
            m_bytesOfRead += dataSize;
        }
        
        dataSize = m_dataSize - m_bytesOfRead;
        if (!m_bExitReadDataTask && dataSize > 0)
        {
            theDataBuffer.mBuffers[0].mDataByteSize = (UInt32)dataSize;
            theDataBuffer.mBuffers[0].mData = m_pPcmData + m_bytesOfRead;
            frames = readInFrames;
            ExtAudioFileRead(extRef, (UInt32*)&frames, &theDataBuffer);
        }
        
        m_bytesOfRead = m_dataSize;
    }
    else
    {
        m_uQueBufferFrames = theFileFormat.mSampleRate * QUEUEBUFFER_TIME_STEP;
        m_uQueBufferBytes = m_uQueBufferFrames * m_outputFormat.mBytesPerFrame;
        
        theDataBuffer.mNumberBuffers = 1;
        theDataBuffer.mBuffers[0].mNumberChannels = m_outputFormat.mChannelsPerFrame;
        
        for (int index = 0; index < QUEUEBUFFER_NUM; ++index)
        {
            m_queBuffers[index] = (char*)malloc(m_uQueBufferBytes);
            theDataBuffer.mBuffers[0].mDataByteSize = m_uQueBufferBytes;
            theDataBuffer.mBuffers[0].mData = m_queBuffers[index];
            frames = m_uQueBufferFrames;
            ExtAudioFileRead(extRef, (UInt32*)&frames, &theDataBuffer);
            m_queBufferSize[index] = theDataBuffer.mBuffers[0].mDataByteSize;
        }
    }
    
ExitThread:
    CFRelease(fileURL);
    if (extRef)
    {
        ExtAudioFileDispose(extRef);
    }
    m_readDataTaskMutex.unlock();
    if(!m_bAlBufferReady)
    {
        m_bAlBufferReady = m_uQueBufferFrames > 0;
    }
    m_bLoadFail = !m_bAlBufferReady;
    InvokingPlayCallbacks();
}

void CAudioCache::ReadDataTask()
{
    if (m_eFileFormat == EFileFormat::OGG)
    {
        ReadOgg();
    }
    else
    {
        ReadOthers();
    }
}

void CAudioCache::AddPlayCallback(const std::function<void()>& callback)
{
    m_callbacks.push_back(callback);
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
