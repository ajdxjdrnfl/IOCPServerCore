#include "pch.h"
#include "Service.h"
#include "IocpCore.h"
#include "Session.h"
#include "Listener.h"

Service::Service(IocpCoreRef iocpCore, NetAddress address, 
	::function<SessionRef(void)> sessionFactory, int maxSessionCount)
	: _iocpCore(iocpCore), _netAddress(address), 
	_sessionFactory(sessionFactory), _maxSessionCount(maxSessionCount)
{
	
}

Service::~Service()
{

}

SessionRef Service::CreateSession()
{
	SessionRef session = _sessionFactory();
	session->SetService(shared_from_this());
	if (_iocpCore->Register(session) == false)
		return nullptr;
	return session;

}

void Service::AddSession(SessionRef session)
{
	
	_sessionCount++;
	_sessions.insert(session);
	
}

void Service::ReleaseSession(SessionRef session)
{
	assert(_sessions.erase(session) != 0);
	_sessionCount--;
	
}

bool ServerService::Start()
{
	if (CanStart() == false)
		return false;

	_listener = make_shared<Listener>();
	
	_listener->
	
}

bool ClientService::Start()
{
	if (CanStart() == false)
		return false;
}
