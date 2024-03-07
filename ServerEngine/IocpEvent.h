#pragma once
enum class IocpEventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	Recv,
	Send
};

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(IocpEventType type);

	void Init();

	IocpEventType GetEventType() { return _type; }
	IocpObjectRef GetOwner() { return _owner; }

	void SetOwner(IocpObjectRef owner) { _owner = owner; }

	vector<BYTE>& GetBuffer() { return _buffer; }

private:
	IocpObjectRef _owner = nullptr;
	IocpEventType _type;

	vector<BYTE> _buffer;
};