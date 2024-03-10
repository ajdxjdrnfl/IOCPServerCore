#pragma once
#include "RawBuffer.h"

class RecvBuffer : public RawBuffer
{
	enum { BUFFER_COUNT = 10};
public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();

	bool OnRead(int32 numOfBytes);
	bool OnWrite(int32 numOfBytes);
	void Clean();

	BYTE* ReadPos() { return &_buffer[_readPos]; }
	BYTE* WritePos() { return &_buffer[_writePos]; }
	int32 DataSize() { return _writePos - _readPos; };
	int32 FreeSize() { return _capacity - _writePos; }

private:
	int32 _capacity = 0;
	int32 _readPos = 0;
	int32 _writePos = 0;

};

