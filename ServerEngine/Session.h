#pragma once
#include "IocpObject.h"
#include "IocpEvent.h"

class Session :
    public IocpObject
{
public:
    Session();
    virtual ~Session();

public:
    void Send();
    bool Connect();
    void Disconnect(const wstring& cause);

    virtual void Dispatch(IocpEvent* iocpEvent, int32 numOfBytes) override;
public:
    bool IsConnected() { return _connected; }

private:
    bool RegisterConnect();
    bool RegisterDisconnect();
    void RegisterSend();
    void RegisterRecv();
    
    void ProcessConnect();
    void ProcessDisconnect();
    void ProcessRecv(int32 numOfBytes);
    void ProcessSend(int32 numOfBytes);

private:
    void HandleError(int32 errorCode);

protected:
    virtual void OnConnected() { }
    virtual void OnRecv(BYTE* buffer, int32 numOfBytes) { }
    virtual void OnSend(int32 numOfBytes) { }
    virtual void OnDisconnected() { }

private:
    atomic<bool> _connected = false;

private:
    IocpEvent _connectEvent;
    IocpEvent _disconnectEvent;
    IocpEvent _sendEvent;
    IocpEvent _recvEvent;

};

