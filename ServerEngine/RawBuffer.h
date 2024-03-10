#pragma once

enum class BufferType
{
	Send,
	Recv,
};

class RawBuffer : enable_shared_from_this<RawBuffer>
{
public:
	RawBuffer(BufferType type) : _type(type){}
	virtual ~RawBuffer() { }

	BYTE* GetRawBuffer() { return _buffer.data(); }
	BufferType GetBufferType() { return _type; }

protected:
	vector<BYTE> _buffer;
	BufferType _type;
	int32 _bufferSize = 0;
};

