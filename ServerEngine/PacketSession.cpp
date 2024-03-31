#include "pch.h"
#include "PacketSession.h"

int32 PacketSession::OnRecv(BYTE* buffer, int32 numOfBytes)
{
	int32 processLen = 0;
	
	while (true)
	{
		int dataSize = numOfBytes - processLen;

		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));

		if (dataSize < header.size)
			break;
		
		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;

	}

	return processLen;
}