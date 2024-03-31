#include "pch.h"
#include "HttpServerService.h"

HttpServerService::HttpServerService(IocpCoreRef iocpCore, NetAddress address, ::function<SessionRef(void)> sessionFactory, int maxSessionCount)
	: Super(iocpCore, address, sessionFactory, maxSessionCount)
{

}

bool HttpServerService::Start()
{
	if (Super::Start() == false)
		return false;

	
}