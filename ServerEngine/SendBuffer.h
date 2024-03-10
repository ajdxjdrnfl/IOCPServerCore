#pragma once
#include "RawBuffer.h"
class SendBuffer : public RawBuffer
{
public:
	SendBuffer(int32 bufferSize);
	~SendBuffer();

	int32 Capacity() { return static_cast<int32>(_buffer.capacity()); }
	int32 WriteSize() { return _writeSize; }

	void CopyData(void* data, int32 len);
	void Close(uint32 writeSize);

private:
	int32 _writeSize = 0;
};

