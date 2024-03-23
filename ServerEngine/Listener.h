#pragma once
#include "IocpObject.h"
class Listener :   public IocpObject
{
public:
	Listener();
	virtual ~Listener();

	virtual void Dispatch(IocpEvent* iocpEvent, int32 numOfBytes) override;
	
public:
	void StartAccept();

private:
	void ProcessAccept(IocpEvent* acceptEvent);

	void RegisterAccept(class IocpEvent* acceptEvent);

private:
	ServerServiceRef _service;
	vector<IocpEvent*> _acceptEvents;
};

