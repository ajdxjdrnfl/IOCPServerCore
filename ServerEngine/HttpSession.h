#pragma once
#include "Session.h"
#include "SendBuffer.h"
#include "RecvBuffer.h"

struct HttpHeader
{
    int32 size;
    
};

enum HttpMethod
{
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,

};


class HttpSession :
    public Session
{
public:
    HttpSession();
    virtual ~HttpSession();

public:
    virtual void Dispatch(IocpEvent* iocpEvent, int32 numOfBytes) override;
    bool Connect();
    void Disconnect();

public:
    void RegisterConnect();
    void RegisterRecv();

protected:
    virtual void OnConnected() { }
    virtual int32 OnRecv(BYTE* buffer, int32 numOfBytes) { return numOfBytes; }
    virtual void OnSend(int32 numOfBytes) { }
    virtual void OnDisconnected() { }

public:
    
private:
    SendBuffer _sendBuffer;
    RecvBuffer _recvBuffer;

};

