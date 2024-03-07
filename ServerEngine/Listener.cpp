#include "pch.h"
#include "Listener.h"
#include "IocpEvent.h"


void Listener::Dispatch()
{
	StartAccept();
}

void Listener::StartAccept()
{
	
	_socket = SocketManager::CreateSocket();

	if (_socket == INVALID_SOCKET)
		return;

	if (SocketManager::SetReuseAddress(_socket, true) == false)
		return;

	if (SocketManager::SetLinger(_socket, 0, 0) == false)
		return;


	if (SocketManager::BindAnyAddress(_socket, 7777) == false)
		return;

	if (SocketManager::Listen(_socket) == false)
		return;

	




	
}

void Listener::RegisterAccept(IocpEvent* acceptEvent)
{

	SocketManager::AcceptEx(_socket, );
}