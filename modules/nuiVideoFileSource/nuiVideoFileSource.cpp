#include "nuiVideoFileSource.h"

nuiVideoFileSourceDataPacket::~nuiVideoFileSourceDataPacket()
{
  if(this->isLocalCopy()) cvReleaseImage(&data);
};

nuiDataPacketError::err nuiVideoFileSourceDataPacket::packData(const void *_data)
{
  this->setLocalCopy(false);
  this->data = (IplImage*)_data;
  return nuiDataPacketError::NoError;
};

nuiDataPacketError::err nuiVideoFileSourceDataPacket::unpackData(void* &_data)
{
  _data = (void*)this->data;
  return nuiDataPacketError::NoError;
};

nuiDataPacket* nuiVideoFileSourceDataPacket::copyPacketData(nuiDataPacketError::err &errorCode)
{
  nuiVideoFileSourceDataPacket* newDataPacket = new nuiVideoFileSourceDataPacket();

  //! TODO : Test if this implies deep copy
  IplImage* newData = cvCloneImage((this->data));

  newDataPacket->packData(newData);
  newDataPacket->setLocalCopy(true);

  errorCode = nuiDataPacketError::NoError;
  return newDataPacket;
};

char* nuiVideoFileSourceDataPacket::getDataPacketType()
{
  return "IplImage";
};

MODULE_DECLARE(VideoFileSource, "native", "Get video from a file source")

  nuiVideoFileSource::nuiVideoFileSource() : nuiModule()
{
  MODULE_INIT();

  this->output = new nuiEndpoint(this);
  this->output->setTypeDescriptor(std::string("IplImage"));
  this->setOutputEndpointCount(1);
  this->setOutputEndpoint(0,this->output);

  this->outputDataPacket = new nuiVideoFileSourceDataPacket();
};

nuiVideoFileSource::~nuiVideoFileSource()
{};

void nuiVideoFileSource::update()
{
  this->output->lock();

  this->output->clear();

  frame = cvQueryFrame(cap);
  if(frame == NULL) {
    cvReleaseCapture(&cap);
    cvReleaseImage(&frame);
    cap = cvCaptureFromAVI(this->property("path").asString().c_str());
    frame = cvQueryFrame(cap);
  }
  CvFont font;
  double hScale=0.5;
  double vScale=0.5;
  int    lineWidth=1;
  cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);
  std::ostringstream oss;
  oss << "~ " << this->timer->getAverageFPS() << " FPS";
  cvRectangle(frame, cvPoint(150,0), cvPoint(300,20), cvScalar(0,0,0), CV_FILLED, CV_AA);
  cvRectangle(frame, cvPoint(150,0), cvPoint(300,20), cvScalar(255,100,100), 2, CV_AA);
  cvPutText (frame, oss.str().c_str(), cvPoint(155,15), &font, cvScalar(255,255,255));

  this->outputDataPacket->packData((void*)frame);
  this->output->setData(this->outputDataPacket);
  this->output->transmitData();
  this->output->unlock();
};

void nuiVideoFileSource::start()
{
  cap = cvCaptureFromAVI(this->property("path").asString().c_str());
  nuiModule::start();
};

void nuiVideoFileSource::stop()
{
  cvReleaseCapture(&cap);
  //cvReleaseImage(&frame);
  nuiModule::stop();
};
