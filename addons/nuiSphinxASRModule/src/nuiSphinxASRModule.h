/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiSpinxASRModule.cpp
// Purpose:     Sphinx3 speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef NUI_SPHINX_ASR_MODULE_H
#define NUI_SPHINX_ASR_MODULE_H

#include "nuiDebugLogger.h"
#include "nuiModule.h"
#include "nuiProperty.h"
#include "nuiDataStream.h"
#include "nuiDataGenericContainer.h"
#include "nuiAudioData.h"
#include "nuiPlugin.h"

#include "s3_decode.h"
#include "fsg_model.h"
#include "jsgf.h"
#include "..\..\..\libs\nuiModule\inc\nuiEndpoint.h"
#include "..\..\..\libs\nuiModule\inc\nuiDataPacket.h"

// The return codes
#define SPHINXASR_SUCCESS                   0
#define SPHINXASR_FAIL_UNKNOWN             -1
#define SPHINXASR_FAIL_READ_FILES          -2
#define SPHINXASR_HAVE_NOT_INIT            -3
#define SPHINXASR_FAIL_WRITE_CONFIG        -4
#define SPHINXASR_INVALID_JSGF_GRAMMAR     -5
#define SPHINXASR_INVALID_CONFIG           -6
#define SPHINXASR_FAIL_INIT_FRONTEND       -7
#define SPHINXASR_FAIL_STARTENGINE         -8
#define SPHINXASR_FAIL_INIT_DECODER        -9
#define SPHINXASR_INVALID_AUDIO_FORMAT     -10
#define SPHINXASR_HAVE_NOT_START           -11

struct ccaSphinxASREngineArgs
{    
    int samplerate;
    
    // Only for sphinx
    std::string sphinxmodel_am;
    std::string sphinxmodel_lm;
    std::string sphinxmodel_dict;
    std::string sphinxmodel_fdict;
    int sphinx_mode;
        
    ccaSphinxASREngineArgs()
    {
        samplerate = 8000;
        sphinx_mode = 0;
    }
};

class ccaSphinxASREngine {

public:
    ccaSphinxASREngine();
    ~ccaSphinxASREngine();
    
    int engineInit(ccaSphinxASREngineArgs *e);
    int engineExit();
    int engineOpen();
    int engineClose();
    int engineSentAudio(short *audioBuf, int audioSize);
    bool isEngineOpened();
    char * engineGetText();
    
private:
    // sphinx3 Decoder
    s3_decode_t *decoder;
    fe_t *fe;
    
    // state
    bool bEngineInitialed;
    bool bEngineOpened;
    
    
    // finite state grammar
    fsg_model_t *get_fsg(jsgf_t *grammar, const char *name);
    
    // utterance number
    int uttnum;
};

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(SphinxASRModule, char*)

class nuiSphinxASRModule : public nuiModule {

public:
	nuiSphinxASRModule();
    ~nuiSphinxASRModule();
    
    //! TODO Do we still need this?
    //void notifyData(nuiDataStream*);
    void update();
    void stop();
    void start();
    
protected:

    //nuiDataStream* output;
    //nuiDataStream* input;
    
    nuiEndpoint* input;
    nuiEndpoint* output;

    nuiDataPacket* inputDataPacket;
    nuiDataPacket* outputDataPacket;

	MODULE_INTERNALS();
    
private:
    // The ASR Engine
    ccaSphinxASREngine *engine;
    int                 model_sampleRate;
};

IMPLEMENT_ALLOCATOR(nuiSphinxASRModule)
IMPLEMENT_DEALLOCATOR(nuiSphinxASRModule)

START_IMPLEMENT_DESCRIPTOR(nuiSphinxASRModule,"native","CCA recognition hypothesis")
descriptor->setInputEndpointsCount(1);
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("AudioDataStream");
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("CCAHypothesis");
descriptor->addInputEndpointDescriptor(inputDescriptor,0);
descriptor->addOutputEndpointDescriptor(outputDescriptor,0);
descriptor->property("use_thread").set(true);
END_IMPLEMENT_DESCRIPTOR(nuiSphinxASRModule)	


START_MODULE_EXIT()
END_MODULE_EXIT()

START_MODULE_REGISTRATION()	  
REGISTER_PLUGIN(nuiSphinxASRModule,"nuiSphinxASRModule",1,0)
END_MODULE_REGISTRATION()

#endif

