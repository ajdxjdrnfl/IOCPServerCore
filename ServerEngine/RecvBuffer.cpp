#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 bufferSize) : _bufferSize(bufferSize)
{
	_capacity = bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);

}

RecvBuffer::~RecvBuffer()
{

}


bool RecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
	{
		return false;
	}

	_readPos += numOfBytes;
	return true;

}

bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
	{
		return false;

	}

	_writePos += numOfBytes;
	return true;

}

void RecvBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		_readPos = _writePos = 0;
	}
	else
	{
		if (FreeSize() < _bufferSize)
		{
			::memcpy(&_buffer[0], ReadPos(), dataSize);
			_writePos = dataSize;
			_readPos = 0;
		}

	}
}