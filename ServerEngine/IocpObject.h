#pragma once
class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	IocpObject() { }
	virtual ~IocpObject() { }

	virtual void Dispatch() abstract;
	HANDLE GetHandle() { return reinterpret_cast<HANDLE>(_socket); }
	virtual void Dispatch(struct IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;

protected:
	SOCKET _socket = INVALID_SOCKET;
};

