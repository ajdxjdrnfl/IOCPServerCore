#pragma once
class Service : public enable_shared_from_this<Service>
{
public:
	Service();
	virtual ~Service();

	virtual bool Start() abstract;
	bool CanStart() { return _sessionFactory != nullptr; }

	virtual void CloseService();
	void SetSessionFactory(::function<SessionRef(void)> func) { _sessionFactory = func; }
	
	SessionRef CreateSession();
	void AddSession(SessionRef session);
	void ReleaseSession(SessionRef session);

	int32 GetCurrentSessionCount() { return _sessionCount; }
	int32 GetMaxSessionCount() { return _maxSessionCount; }
private:
	IocpCoreRef _iocpCore;

	set<SessionRef> _sessions;
	int32 _sessionCount = 0;
	int32 _maxSessionCount = 0;
	::function<SessionRef(void)> _sessionFactory;

};


class ServerService : public Service
{
	using Super = Service;
public:
	ServerService();
	virtual ~ServerService();
	virtual bool Start() override;

private:
	shared_ptr<class Listener> _listener = nullptr;
};

class ClientService : public Service

{
	using Super = Service;
public:
	ClientService();
	virtual ~ClientService();
	virtual bool Start() override;
};