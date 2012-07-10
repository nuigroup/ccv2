/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiAudioOutputModule.cpp
// Purpose:     Uses RtAudio library to collect audio input
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "nuiAudioOutputModule.h"


nuiAudioOutputModuleDataPacket::~nuiAudioOutputModuleDataPacket()
{
    if(this->isLocalCopy())
    {
        free(data->buffer);
        delete data;
    }
    nuiDataPacket::~nuiDataPacket();
}

nuiDataPacketError nuiAudioOutputModuleDataPacket::packData(const void *_data)
{
	this->setLocalCopy(false);
    this->data = (AudioDataStream*)_data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiAudioOutputModuleDataPacket::unpackData(void* &data)
{
    data = (void*)this->data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiAudioOutputModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
    nuiDataPacket* packet = new nuiAudioOutputModuleDataPacket();
    
    AudioDataStream* currentData = this->data;
    AudioDataStream* newData = new AudioDataStream();
    //deep copy
    newData->bufferSize = currentData->bufferSize;
    newData->sampleRate = currentData->sampleRate;
    newData->buffer = (short*)malloc(newData->bufferSize);
    memcpy(newData->buffer, currentData->buffer, newData->bufferSize);
    
    packet->packData(newData);
    packet->setLocalCopy(true);
    
    return packet;
};

char* nuiAudioOutputModuleDataPacket::getDataPacketType()
{
    return "AudioDataStream";
}

MODULE_DECLARE(AudioOutput, "native", "Get audio from RtAudio input (Core Audio or DirectSound)");

bool nuiAudioOutputModule::bRecording = false;

nuiAudioOutputModule::nuiAudioOutputModule() : nuiModule() 
{
    
    MODULE_INIT();
    
    resample_handle = NULL;
    
    resample_factor = AUDIO_TARGET_SAMPLE_RATE_F / AUDIO_HARDWARE_SAMPLE_RATE_F;
    resample_handle = resample_open(1, resample_factor, resample_factor);
    
    audioBuf = NULL;
    audioBufSize = 0;
    

	this->output = new nuiEndpoint();
	this->output->setTypeDescriptor(std::string("AudioDataStream"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0,this->output);
	this->output->setModuleHoster(this);
	//this->declareOutput(0, &this->output, new nuiDataStreamInfo("AudioDataStream", "audiodatastream_8000", "AudioDataStream (samplerate: 8000, format: 16-bit/short)"));
    
	this->input = new nuiEndpoint();
	this->input->setTypeDescriptor(std::string("AudioTriggers"));
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0,this->input);
	this->input->setModuleHoster(this);

	inputDataPaket = NULL;
	outputDataPaket = new nuiAudioOutputModuleDataPacket();

//    this->declareInput(0, &this->input, new nuiDataStreamInfo("AudioTriggers", "AudioTriggers", "Audio triggers (basically just a bool for recording)"));
    
    this->properties["use_thread"] = new nuiProperty(true);
}

nuiAudioOutputModule::~nuiAudioOutputModule() 
{
    
}

void nuiAudioOutputModule::stop() 
{
    nuiModule::stop();
}

void nuiAudioOutputModule::start() 
{
    nuiModule::start();
}

AudioDataStream* nuiAudioOutputModule::recordAudioEnd() 
{
	unsigned int channels = 1, fs = AUDIO_HARDWARE_SAMPLE_RATE_I, bufferFrames, offset = 0;
	
	RtAudio::StreamParameters oParams;
	oParams.deviceId = recorder.getDefaultOutputDevice();
    oParams.nChannels = channels;
    oParams.firstChannel = offset;

	std::cout << "Current API: " << recorder.getCurrentApi() << std::endl;
	std::cout << "Latency: " << recorder.getStreamLatency() << std::endl;
	

    if(bRecording) {
        LOG(NUI_DEBUG, "done recording");
		bRecording = false;
		try {
			recorder.stopStream();
		} catch (RtError& e) {
			LOG(NUI_ERROR, e.getMessage());
		}
        LOG(NUI_DEBUG, "stopped stream");
        recorder.closeStream();
        LOG(NUI_DEBUG, "closed input stream");
        
        int srcused = 0;
        
        audioBufSize = (unsigned int)data.frameCounter * (unsigned int)data.channels;
        
        int newBufSize = audioBufSize;
        
        data.frameCounter = 0;
        
#ifdef WIN32
        float* buf_f = new float[newBufSize];
        for (int i=0; i<newBufSize; i++) {
			buf_f[i] = AUDIO_SHORT2FLOAT(data.buffer[i]);
        }
		if (fs != AUDIO_TARGET_SAMPLE_RATE_I) {
            newBufSize = int(audioBufSize * resample_factor);
            audioBuf = new float[newBufSize];
            resample_process(resample_handle, resample_factor, buf_f, audioBufSize, 1, &srcused, audioBuf, newBufSize);
        }
#else
        if (fs != AUDIO_TARGET_SAMPLE_RATE_I) {
            newBufSize = int(audioBufSize * resample_factor);
            audioBuf = new float[newBufSize];
            resample_process(resample_handle, resample_factor, data.buffer, audioBufSize, 1, &srcused, audioBuf, newBufSize);
        }

#endif
        short* buf_16 = new short[newBufSize];
        for (int i=0; i<newBufSize; i++) {
            buf_16[i] = AUDIO_FLOAT2SHORT(audioBuf[i]);
        }

        AudioDataStream* outputStream = new AudioDataStream;
        outputStream->buffer = buf_16;
        outputStream->sampleRate = AUDIO_TARGET_SAMPLE_RATE_I;
        outputStream->bufferSize = newBufSize;
                
        return outputStream;
    } else {
        return NULL;
    }
    
}

void nuiAudioOutputModule::recordAudioStart(int max_time = 3) 
{
	recorder.showWarnings();
    unsigned int channels = 1, fs = AUDIO_HARDWARE_SAMPLE_RATE_I, bufferFrames, offset = 0;
    bRecording = true;
    this->output->lock();
    
    if ( recorder.getDeviceCount() < 1 )
	{
        LOG(NUI_ERROR, "No audio devices found!");
        bRecording = false;
        this->output->unlock();
    }
    
    recorder.showWarnings( true );
    bufferFrames = 512;
    RtAudio::StreamParameters iParams;
	iParams.deviceId = recorder.getDefaultInputDevice();
    iParams.nChannels = channels;
    iParams.firstChannel = offset;
    
    data.buffer = 0;

	options.flags = RTAUDIO_MINIMIZE_LATENCY || RTAUDIO_SCHEDULE_REALTIME;
        
    try 
	{
        recorder.openStream(NULL, &iParams, FORMAT, fs, &bufferFrames, &gotAudioInput, (void *) &data, &options);
    }
    catch(RtError& e) {
        LOG(NUI_ERROR, e.getMessage());
        bRecording = false;
        this->output->unlock();
    }
    
    data.bufferBytes = bufferFrames * channels * sizeof( AUDIO_TYPE );
    data.totalFrames = (unsigned long) (fs * max_time);
    data.frameCounter = 0;
    data.channels = channels;
    
    unsigned long totalBytes;
    totalBytes = data.totalFrames * channels * sizeof( AUDIO_TYPE );
    
    data.buffer = (AUDIO_TYPE*) malloc(totalBytes);
    
    if(data.buffer == 0) {
        LOG(NUI_ERROR, "malloc problem!");
        this->output->unlock();
        bRecording = false;
    }
    
    try {
        recorder.startStream();
    } catch (RtError& e) {
        LOG(NUI_ERROR, e.getMessage());
        bRecording = false;
        this->output->unlock();
    }
    
    
	if(bRecording) LOG(NUI_DEBUG, "started recording");
    
    
}

void nuiAudioOutputModule::update()
{
    
    LOG(NUI_DEBUG, "got update call");
    int dataSize;
    if(this->input->getData() != NULL)
	{
    
        if(((nuiDataGenericContainer *)this->input->getData())->hasProperty("recording") && !bRecording) 
		{
            this->output->clear();
            recordAudioStart();
        }
        else if(bRecording) 
		{
            AudioDataStream* audio = recordAudioEnd();
            if(audio != NULL) 
			{
				this->outputDataPaket->packData((void*)audio);
				this->output->setData(this->outputDataPaket);
				this->output->transmitData();
                //this->output->push(audio);
            }
			else 
			{
                this->output->clear();
            }
        }
        else 
		{
            this->output->clear();
        }
        
    }
    else 
	{
        this->output->clear();
    }
    
    this->output->unlock();
    
    
}

bool nuiAudioOutputModule::isRecording() 
{
	return bRecording;
}

void nuiAudioOutputModule::setRecording(bool rec)
{
	bRecording = rec;
}

int gotAudioInput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                  double streamTime, RtAudioStreamStatus status, void *data )
{
    AudioData *iData = (AudioData *) data;

    // Simply copy the data to our allocated buffer.
    unsigned int frames = nBufferFrames;
    if ( iData->frameCounter + nBufferFrames > iData->totalFrames ) {
        frames = iData->totalFrames - iData->frameCounter;
        iData->bufferBytes = frames * iData->channels * sizeof( AUDIO_TYPE );
    }
    
    unsigned long offset = iData->frameCounter * iData->channels;
    memcpy( iData->buffer+offset, inputBuffer, iData->bufferBytes );
    iData->frameCounter += frames;
    
    if ( iData->frameCounter >= iData->totalFrames ) return 2;
    return 0;
}

int gotAudioOutput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                   double streamTime, RtAudioStreamStatus status, void *data )
{
    AudioData *aData = (AudioData*) data;

    unsigned int frames = nBufferFrames;
    if ( aData->frameCounter + nBufferFrames > aData->totalFrames ) {
        frames = aData->totalFrames - aData->frameCounter;
        aData->bufferBytes = frames * aData->channels * sizeof( AUDIO_TYPE );
    }
    unsigned long offset = aData->frameCounter * aData->channels;
    memcpy(outputBuffer, aData->buffer+offset, aData->bufferBytes);
    aData->frameCounter += frames;
    if ( aData->frameCounter >= aData->totalFrames ) return 1;
    return 0;
}

