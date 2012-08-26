////////////////////////////////////////////////////////////////////////////
// Name:        nuiDataPacket.h
// Author:      Anatoly Churikov
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef _NUI_DATA_PACKET_
#define _NUI_DATA_PACKET_

typedef enum nuiDataPacketError
{
	NUI_DATAPACKET_OK		= 0x00000000,
	NUI_DATAPACKET_ERROR	= 0x00000001,
};

#define NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(moduleName, datatype) \
class nui##moduleName##DataPacket : public nuiDataPacket \
{ \
public: \
    ~nui##moduleName##DataPacket(); \
    nuiDataPacketError packData(const void *data); \
    nuiDataPacketError unpackData(void* &data); \
    nuiDataPacket*     copyPacketData(nuiDataPacketError &errorCode); \
    char*              getDataPacketType(); \
private: \
    datatype## data; \
}; \

class nuiDataPacket
{
public:
	virtual ~nuiDataPacket() = 0;
	virtual nuiDataPacketError  packData(const void *data) = 0;
    virtual nuiDataPacketError  unpackData(void* &data) = 0;
	virtual nuiDataPacket*      copyPacketData(nuiDataPacketError &errorCode) = 0;

    virtual char*               getDataPacketType() = 0;

    virtual bool                isLocalCopy(){ return localCopy; };
    virtual void                setLocalCopy(bool value){ localCopy = value; };
private:
    bool localCopy;
}; 

#endif//_NUI_DATA_PACKET_