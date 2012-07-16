/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiSphinxASRModule.cpp
// Purpose:     Sphinx3 speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "nuiSphinxASRModule.h"
#include "nuiDebugLogger.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include "..\..\..\libs\nuiModule\inc\nuiEndpoint.h"

nuiSphinxASRModuleDataPacket::~nuiSphinxASRModuleDataPacket()
{
    if(this->isLocalCopy())
    {
        free(this->data);
    }
};

nuiDataPacketError nuiSphinxASRModuleDataPacket::packData(const void *_data)
{
    this->setLocalCopy(false);
    this->data = (char*) _data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiSphinxASRModuleDataPacket::unpackData(void* &_data)
{
    _data = (void*)this->data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiSphinxASRModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
    nuiDataPacket* newDataPacket = new nuiSphinxASRModuleDataPacket();
    char* currentData = this->data;
    char* newData = (char*)malloc(strlen(this->data));
    strcpy(newData,currentData);

    newDataPacket->packData(newData);
    newDataPacket->setLocalCopy(true);
    errorCode = NUI_DATAPACKET_OK;
    return newDataPacket;
};

char* nuiSphinxASRModuleDataPacket::getDataPacketType()
{
    return "CCAHypothesis";
};


MODULE_DECLARE(SphinxASR, "native", "Fetch CCA speech recognition");

nuiSphinxASRModule::nuiSphinxASRModule() : nuiModule() {

	MODULE_INIT();

	this->properties["acoustic_model"] = new nuiProperty("configs/ballworld/sphinxmodel/.");
	this->properties["language_model"] = new nuiProperty("configs/ballworld/sphinxmodel/voxforge_en_sphinx.lm.DMP");
	this->properties["dictionary"] = new nuiProperty("configs/ballworld/sphinxmodel/cmudict.0.7a");
	this->properties["noise_dictionary"] = new nuiProperty("configs/ballworld/sphinxmodel/noisedict");

    /*this->output = new nuiDataStream("CCAHypothesis");
    this->declareOutput(0, &this->output, new nuiDataStreamInfo("CCAHypothesis", "CCAHypothesis", "CCA recognition hypothesis"));
    
    this->input = new nuiDataStream("audiodatastream_8000");
    this->declareInput(0, &this->input, new nuiDataStreamInfo("AudioDataStream", "audiodatastream_8000", "AudioDataStream (samplerate: 8000, format: 16-bit/short)"));*/
    
    this->output = new nuiEndpoint();
    this->output->setTypeDescriptor(std::string("CCAHypothesis"));
    this->setOutputEndpointCount(1);
    this->setOutputEndpoint(0,this->output);
    this->output->setModuleHoster(this);

    this->input = new nuiEndpoint();
    this->input->setTypeDescriptor(std::string("AudioDataStream"));
    this->setInputEndpointCount(1);
    this->setInputEndpoint(0,this->input);
    this->input->setModuleHoster(this);



    engine = NULL;
    model_sampleRate = 8000;
        
	// declare properties here, e.g:
	// this->properties["size"] = new moProperty(1.);
}

nuiSphinxASRModule::~nuiSphinxASRModule() {
    delete engine;
}

void nuiSphinxASRModule::start() {
    
    // ASR Engine: commandpicking
    engine = new ccaSphinxASREngine();
    ccaSphinxASREngineArgs *engineArgs = new ccaSphinxASREngineArgs;
    
	engineArgs->sphinxmodel_am = this->properties["acoustic_model"]->asString();
    engineArgs->sphinxmodel_lm = this->properties["language_model"]->asString();
    engineArgs->sphinxmodel_dict = this->properties["dictionary"]->asString();
    engineArgs->sphinxmodel_fdict = this->properties["noise_dictionary"]->asString();
    
    engineArgs->sphinx_mode = 2;
    engineArgs->samplerate = model_sampleRate;
    int asrstatus = engine->engineInit(engineArgs);
    if (asrstatus != SPHINXASR_SUCCESS) {
        LOG(NUI_DEBUG, "Initialization of ASR Engine failed!");
        LOG(NUI_DEBUG, asrstatus);
    }
    
    delete engineArgs;
    
    nuiModule::start();
    
}

void nuiSphinxASRModule::stop() {
    engine->engineExit();
    nuiModule::stop();
}

//! TODO Do we still need this?
/*void nuiSphinxASRModule::notifyData(nuiDataStream* data) {
    assert(data != NULL);
    assert(data == this->input);
    this->notifyUpdate();
}*/

void nuiSphinxASRModule::update() {    
    engine->engineOpen();
    
    this->input->lock();
    
    AudioDataStream* ad = (AudioDataStream*)this->input->getData();
    
    this->input->unlock();
    
    if(ad != NULL) {
    
        if(engine->engineSentAudio(ad->buffer, ad->bufferSize) != SPHINXASR_SUCCESS) {
            LOG(NUI_DEBUG, "recognize failed");
            engine->engineClose();
            this->output->clear();
        } else {
            LOG(NUI_DEBUG, "recogize succeeded!");
            std::string hypothesis(engine->engineGetText());
            engine->engineClose();
            std::transform(hypothesis.begin(), hypothesis.end(), hypothesis.begin(), ::tolower);
            char* chyp = new char[hypothesis.size()+1];
            strcpy (chyp, hypothesis.c_str());
            //this->output->push((void*)chyp);
            this->outputDataPacket->packData((void*)chyp);
        }
    }
    else {
        this->output->clear();
    }
    
}

// ******** the engine ********

ccaSphinxASREngine::ccaSphinxASREngine() {
    
    bEngineInitialed = false;
    bEngineOpened = false;
    decoder = NULL;
    fe = NULL;
    uttnum = 0;
    
}

ccaSphinxASREngine::~ccaSphinxASREngine() {
    if(this->isEngineOpened()) this->engineClose();
    if(this->bEngineInitialed) this->engineExit();
}

int ccaSphinxASREngine::engineInit(ccaSphinxASREngineArgs *args) {
#if defined WIN32
    char cfg_filename[] = "sphinx.cfg";
    char grammarFSG_filename[] = "configs\\ballworld\\ballworld.fsg";
#else
	char cfg_filename[] = "/tmp/sphinx.cfg";
    char grammarFSG_filename[] = "configs/ballworld/ballworld.fsg";
#endif	
    FILE *cfg_fp = fopen(cfg_filename, "wt");
    if (cfg_fp==NULL)
        return SPHINXASR_FAIL_WRITE_CONFIG;
    if ( access(args->sphinxmodel_am.c_str(), 0) != 0 )
		return SPHINXASR_FAIL_READ_FILES;
	if ( access(args->sphinxmodel_lm.c_str(), 0) != 0 )
		return SPHINXASR_FAIL_READ_FILES;
	if ( access(args->sphinxmodel_dict.c_str(), 0) != 0 )
		return SPHINXASR_FAIL_READ_FILES;
	if ( access(args->sphinxmodel_fdict.c_str(), 0) != 0 )
		return SPHINXASR_FAIL_READ_FILES;
	
	/*char cur_path[1024];
	getcwd(cur_path, 1024);*/
    
    fprintf(cfg_fp, "-samprate %d\n", args->samplerate);
    fprintf(cfg_fp, "-hmm %s\n", args->sphinxmodel_am.c_str());
    fprintf(cfg_fp, "-dict %s\n", args->sphinxmodel_dict.c_str());
    fprintf(cfg_fp, "-fdict %s\n", args->sphinxmodel_fdict.c_str());
    fprintf(cfg_fp, "-lm %s\n", args->sphinxmodel_lm.c_str());
    if (args->sphinx_mode == 2) {
        fprintf(cfg_fp, "-fsg %s\n", grammarFSG_filename);
    }
    fprintf(cfg_fp, "-op_mode %d\n", args->sphinx_mode);
    fclose(cfg_fp);
    err_set_logfp(NULL); // disable logs
    cmd_ln_t *config = NULL;
    config = cmd_ln_parse_file_r(config, S3_DECODE_ARG_DEFS, cfg_filename, TRUE);
    if (config == NULL) {
        return SPHINXASR_INVALID_CONFIG;
    }
    decoder = new s3_decode_t;
    if (s3_decode_init(decoder, config) != S3_DECODE_SUCCESS) {
        return SPHINXASR_FAIL_INIT_DECODER;
    }
    fe = fe_init_auto_r(config);
    if (fe == NULL) {
        return SPHINXASR_FAIL_INIT_FRONTEND;
    }
    bEngineInitialed = true;
    return SPHINXASR_SUCCESS;

}

int ccaSphinxASREngine::engineExit() {
    s3_decode_close(decoder);
    if (decoder != NULL) {
        delete decoder;
        decoder = NULL;
    }
    if (fe != NULL) {
        fe_free(fe);
        fe = NULL;
    }
    bEngineInitialed = false;
    return SPHINXASR_SUCCESS;

}

int ccaSphinxASREngine::engineOpen()
{
    if (! bEngineInitialed)
        return SPHINXASR_HAVE_NOT_INIT;
    char uttid[8];
    sprintf(uttid, "%d", uttnum);
    if (s3_decode_begin_utt(decoder, uttid) != S3_DECODE_SUCCESS)
    {
        return SPHINXASR_FAIL_STARTENGINE;
    }
    else {
        bEngineOpened = true;
        return SPHINXASR_SUCCESS;
    }
}

int ccaSphinxASREngine::engineClose()
{
    bEngineOpened = false;
    if (! bEngineInitialed)
        return SPHINXASR_SUCCESS;
    s3_decode_end_utt(decoder);
    return SPHINXASR_SUCCESS;
}

int ccaSphinxASREngine::engineSentAudio(short *audioBuf, int audioSize)
{
    float32 **frames;
    int n_frames;
    if (!bEngineInitialed)
        return SPHINXASR_HAVE_NOT_INIT;
    if (!bEngineOpened)
        return SPHINXASR_HAVE_NOT_START;
    fe_process_utt(fe, audioBuf, audioSize, &frames, &n_frames);
    if (frames != NULL) {
        s3_decode_process(decoder, frames, n_frames);
        ckd_free_2d((void **)frames);
    }
    return SPHINXASR_SUCCESS;
}

char * ccaSphinxASREngine::engineGetText()
{
    char * hypstr;
    hyp_t **hypsegs;
    if (s3_decode_hypothesis(decoder, NULL, &hypstr, &hypsegs)
        == S3_DECODE_SUCCESS && decoder->phypdump) {
        /*for (; *hypsegs; hypsegs++) {
            printf("Word-segment id: %i; start-frame: %i; string: %s; score: %i\n", (*hypsegs)->id, (*hypsegs)->sf, (*hypsegs)->word, (*hypsegs)->ascr);
        }*/
        return hypstr;
    }
    else {
        return NULL;
    }
}

bool ccaSphinxASREngine::isEngineOpened()
{
    return bEngineInitialed * bEngineOpened;
}