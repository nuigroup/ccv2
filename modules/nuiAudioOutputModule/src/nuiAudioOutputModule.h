/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiAudioOutputModule.h
// Purpose:     Uses RtAudio library to collect audio input
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef NUI_AUDIO_OUTPUT_MODULE_H
#define NUI_AUDIO_OUTPUT_MODULE_H

#include "nuiDebugLogger.h"
#include "nuiModule.h"
#include "nuiProperty.h"
#include "nuiDataStream.h"
#include "nuiDataGenericContainer.h"
#include "nuiPlugin.h"
#include "RtAudio.h"
#include "libresample.h"
#include "nuiAudioData.h"
#include "nuiEndpoint.h"
#include "nuiDataPacket.h"

/*class nuiAudioOutputModuleDataPacket : public nuiDataPacket
{
public:
                        ~nuiAudioOutputModuleDataPacket() ;
    nuiDataPacketError  packData(const void *data);
    nuiDataPacketError  unpackData(void* &data);
    nuiDataPacket*      copyPacketData(nuiDataPacketError &errorCode);
    char*               getDataPacketType();

private:
    AudioDataStream*    data;
};*/
NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(AudioOutputModule, AudioDataStream*);

class nuiAudioOutputModule : public nuiModule 
{
public:
    nuiAudioOutputModule();
    ~nuiAudioOutputModule();
    void start();
    void update();
    void stop();
	static bool isRecording();
	static void setRecording(bool rec);
    
protected:
    void *              resample_handle;
    float               resample_factor;
    
    RtAudio recorder;
    
    static bool         bRecording;
    
    float*              audioBuf;
    unsigned int        audioBufSize;

	//int gotAudioInput(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data);
    
    AudioData data;
    
	RtAudio::StreamOptions options;
    
    nuiEndpoint* output;
    nuiEndpoint* input;
    
	nuiDataPacket* inputDataPaket;
	nuiDataPacket* outputDataPaket;

    void recordAudioStart(int max_time);
    AudioDataStream* recordAudioEnd();
    
    MODULE_INTERNALS();
};

int gotAudioInput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data );
int gotAudioOutput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data );

IMPLEMENT_ALLOCATOR(nuiAudioOutputModule)
IMPLEMENT_DEALLOCATOR(nuiAudioOutputModule)

START_IMPLEMENT_DESCRIPTOR(nuiAudioOutputModule,"native","Get audio from RtAudio input (Core Audio or DirectSound)")
descriptor->setInputEndpointsCount(1);
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("AudioTriggers");
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("AudioDataStream");
descriptor->addInputEndpointDescriptor(inputDescriptor,0);
descriptor->addOutputEndpointDescriptor(outputDescriptor,0);
descriptor->property("use_thread").set(true);
END_IMPLEMENT_DESCRIPTOR(nuiAudioOutputModule)	

START_MODULE_EXIT()
END_MODULE_EXIT()

START_MODULE_REGISTRATION()
REGISTER_PLUGIN(nuiAudioOutputModule,"nuiAudioOutputModule",1,0)
END_MODULE_REGISTRATION()

#endif


