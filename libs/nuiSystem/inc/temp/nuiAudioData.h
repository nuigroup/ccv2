/* nuiAudioData.h
*  
*  Created on 05/01/12.
*  Copyright 2012 NUI Group. All rights reserved.
*  Community Core Fusion
*  Author: Anatoly Churikov
*
*/

#ifndef _NUI_AUDIO_DATA_
#define _NUI_AUDIO_DATA_

#if defined(WIN32)
#include <windows.h>
#include <io.h>
#include <direct.h>
#define getcwd _getcwd
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

#define AUDIO_FLOAT2SHORT(P) ( short(P * 32767.0f) )
#define AUDIO_SHORT2FLOAT(P) ( (float(P)) / 32767.0f )

#define AUDIO_SEGBUF_SIZE 256

#define AUDIO_HARDWARE_SAMPLE_RATE_F 44100.0f
#define AUDIO_HARDWARE_SAMPLE_RATE_I 44100

#define AUDIO_TARGET_SAMPLE_RATE_F 8000.0f
#define AUDIO_TARGET_SAMPLE_RATE_I 8000

#ifdef WIN32
typedef short AUDIO_TYPE;
#define FORMAT RTAUDIO_SINT16
#else
typedef float  AUDIO_TYPE;
#define FORMAT RTAUDIO_FLOAT32
#endif


struct AudioData 
{
    AUDIO_TYPE* buffer;
    unsigned long bufferBytes;
    unsigned long totalFrames;
    unsigned long frameCounter;
    unsigned int channels;
};

struct AudioDataStream 
{
    short* buffer;
    int sampleRate;
    unsigned int bufferSize;
};

#endif//_NUI_AUDIO_DATA_