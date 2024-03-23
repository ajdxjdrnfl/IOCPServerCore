#include "pch.h"
#include "Listener.h"
#include "IocpEvent.h"
#include "Service.h"
#include "Session.h"


Listener::Listener()
{

}

Listener::~Listener()
{

}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	assert(iocpEvent->GetEventType() == IocpEventType::Accept);
	ProcessAccept(iocpEvent);
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


	IocpEvent* acceptEvent = new IocpEvent(IocpEventType::Accept);
	acceptEvent->SetOwner(shared_from_this());
	_acceptEvents.push_back(acceptEvent);
	RegisterAccept(acceptEvent);
}

void Listener::RegisterAccept(IocpEvent* acceptEvent)
{
	SessionRef session = _service->CreateSession();

	acceptEvent->Init();
	acceptEvent->SetOwner(session);

	DWORD bytesReceived = 0;
	if (SocketManager::AcceptEx(_socket, session->GetSocket(), session->GetRecvBuffer().WritePos(),
		0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		&bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)) == false)
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			RegisterAccept(acceptEvent);
		}
	}
}

void Listener::ProcessAccept(IocpEvent* acceptEvent)
{
	SessionRef session = static_pointer_cast<Session>(acceptEvent->GetOwner());

	if (SocketManager::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);

	// check socket available
	// can replace "getpeername" with "getsocketname"

	if (::getpeername(session->GetSocket(), reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr) == SOCKET_ERROR)
	{
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetNetAddress(NetAddress(sockAddress));

	// re-use accpetEvent

	RegisterAccept(acceptEvent);
	session->ProcessConnect();
	
}
