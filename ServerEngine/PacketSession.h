#pragma once
#include "Session.h"

struct PacketHeader
{
	uint16 size;
	uint16 id;
};

class PacketSession : public Session
{
public:
	PacketSession() { }
	virtual ~PacketSession() { }

protected:
	virtual int32 OnRecv(BYTE* buffer, int32 numOfBytes);
	virtual void OnSend(int32 numOfBytes) { }

private:
	// Call after complete packet
	virtual void OnRecvPacket(BYTE* buffer, int32 len) abstract; 
};

