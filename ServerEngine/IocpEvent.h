#pragma once
enum class IocpEventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	Recv,
	Send
};

class RawBuffer;

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(IocpEventType type);

	void Init();

	IocpEventType GetEventType() { return _type; }
	IocpObjectRef GetOwner() { return _owner; }

	void SetOwner(IocpObjectRef owner) { _owner = owner; }

	void AddBuffer(RawBufferRef buffer)
	{
		_buffers.push_back(buffer);
	}

	int32 GetBufferLength()
	{
		return _buffers.size();
	}

	vector<RawBufferRef> GetBuffers() { return _buffers; }

private:
	IocpObjectRef _owner = nullptr;
	IocpEventType _type;

	vector<RawBufferRef> _buffers;
	int32 _bufferSize;
};