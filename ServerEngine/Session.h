#pragma once
#include "IocpObject.h"
#include "IocpEvent.h"
#include "RecvBuffer.h"
#include "NetAddress.h"

class Session :
    public IocpObject
{
    enum
    {
        RECV_BUFFER_SIZE = 0x10000, // 4KB
        SEND_BUFFER_SIZE = 0x10000, // 4KB
    };

public:
    Session();
    virtual ~Session();

public:
    void Send(SendBufferRef sendBuffer);
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

    void RemoveIocpEvent(IocpEvent* iocpEvent);
    void AddIocpEvent(IocpEvent* iocpEvent);

    void PushSendBuffer(SendBufferRef sendBuffer);
    void PopAllSendBuffer(IocpEvent* iocpEvent);

protected:
    virtual void OnConnected() { }
    virtual int32 OnRecv(BYTE* buffer, int32 numOfBytes) { return numOfBytes; }
    virtual void OnSend(int32 numOfBytes) { }
    virtual void OnDisconnected() { }

public:
    void SetNetAddress(NetAddress netAddress) { _netAddress = netAddress; }
    void SetService(ServiceRef service) { _service = service; }

public:
    ServiceRef GetService() { return _service; }
    SessionRef GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
    NetAddress GetNetAddress() { return _netAddress; }
    RecvBuffer& GetRecvBuffer() { return _recvBuffer; }

private:
    atomic<bool> _connected = false;
    
private:
    USE_LOCK(SEND)
    RecvBuffer _recvBuffer;
    queue<SendBufferRef> _sendQueue;

    atomic<bool> _sendRegistered = false;

private:
    USE_LOCK(EVENT)
    vector<IocpEvent*> _iocpEvents;
    
    ServiceRef _service;
    NetAddress _netAddress;
    

};

