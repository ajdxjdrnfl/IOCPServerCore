#pragma once
class SendBuffer : public enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(int32 bufferSize);
	~SendBuffer();

	BYTE* GetBuffer() { return _buffer.data(); }
	int32 Capacity() { return static_cast<int32>(_buffer.capacity()); }
	int32 WriteSize() { return _writeSize; }

	void CopyData(void* data, int32 len);
	void Close(uint32 writeSize);

private:
	vector<BYTE> _buffer;
	int32 _writeSize = 0;
};

