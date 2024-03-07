#pragma once
#include "IocpObject.h"
class Listener :   public IocpObject
{
public:
	Listener();
	virtual ~Listener();

	virtual void Dispatch() override;
	
private:
	void StartAccept();
	void ProcessAccept();
	void RegisterAccept(class IocpEvent* acceptEvent);

private:
	
};

