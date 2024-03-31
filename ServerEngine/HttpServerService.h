#pragma once
#include "Service.h"
class HttpServerService :
    public ServerService
{
    using Super = ServerService;
public:
    HttpServerService(IocpCoreRef iocpCore, NetAddress address, ::function<SessionRef(void)> sessionFactory, int maxSessionCount = 1);
    virtual ~HttpServerService() { }

    virtual bool Start();
};

