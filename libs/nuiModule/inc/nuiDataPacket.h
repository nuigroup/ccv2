/** 
 * \file      nuiDataPacket.h
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \date      2012-2013
 * \copyright Copyright 2011 NUI Group. All rights reserved.
 */

#ifndef NUI_DATA_PACKET
#define NUI_DATA_PACKET

//! namespaced enum for errors that Datapacket can produce
struct nuiDataPacketError
{
  enum err
  {
    NoError,
    Error,
  };
};

//! macro for default datapacket skeleton implementation
#define NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(moduleName, datatype) \
class nui##moduleName##DataPacket : public nuiDataPacket \
{ \
public: \
  ~nui##moduleName##DataPacket(); \
  nuiDataPacketError::err packData(const void *data); \
  nuiDataPacketError::err unpackData(void* &data); \
  nuiDataPacket* copyPacketData(nuiDataPacketError::err &errorCode); \
  char* getDataPacketType(); \
private: \
  datatype## data; \
}; \

//! \class datapacket interface. 
//! DataPacket is data wrapper used to transfer data from one module to another
class nuiDataPacket
{
public:
  virtual ~nuiDataPacket() { };
  virtual nuiDataPacketError::err packData(const void *data) = 0;
  virtual nuiDataPacketError::err unpackData(void* &data) = 0;
  virtual nuiDataPacket* copyPacketData(nuiDataPacketError::err &errorCode) = 0;

  virtual char* getDataPacketType() = 0;

  //! checks whether data from one module should be copied or passed by reference
  virtual bool isLocalCopy(){ return localCopy; };

  virtual void setLocalCopy(bool value){ localCopy = value; };
private:
  bool localCopy;
}; 

#endif// NUI_DATA_PACKET