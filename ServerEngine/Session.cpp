#include "pch.h"
#include "Session.h"

Session::Session() : _connectEvent(IocpEventType::Connect), _disconnectEvent(IocpEventType::Disconnect)
, _sendEvent(IocpEventType::Send), _recvEvent(IocpEventType::Recv)
{
	
}

Session::~Session()
{

}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->GetEventType())
	{
	case IocpEventType::Connect:
		ProcessConnect();
		break;
	case IocpEventType::Disconnect:
		ProcessDisconnect();
		break;
	case IocpEventType::Recv:
		ProcessRecv();
		break;
	case IocpEventType::Send:
		ProcessSend();
		break;

	}
}

bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;

	if (SocketManager::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketManager::BindAnyAddress(_socket, 0) == false)
		return false;

	_connectEvent.Init();
	_connectEvent.SetOwner(shared_from_this());

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (false == SocketManager::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_connectEvent.SetOwner(nullptr);
			return false;
		}
	}
	return true;
	
}

bool Session::RegisterDisconnect()
{
	if (!IsConnected()) return false;

	_disconnectEvent.Init();
	_disconnectEvent.SetOwner(shared_from_this());

	if (false == SocketManager::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_disconnectEvent.SetOwner(nullptr);
			return false;
		}
	}

	return true;
}
void Session::RegisterSend()
{
	if (IsConnected() == false)
		return;

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_sendEvent.GetBuffer().data());
	wsaBuf.len = (ULONG)(_sendEvent.GetBuffer().size());

	DWORD sendLen = 0;
	DWORD flags = 0;

	_sendEvent.Init();
	_sendEvent.SetOwner(shared_from_this());

	DWORD numOfBytes = 0;
	if (::WSASend(_socket, &wsaBuf, 1, &numOfBytes, 0, &_sendEvent, nullptr) == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_sendEvent.SetOwner(nullptr);
			_sendEvent.GetBuffer().clear();

		}
	}
}
void Session::RegisterRecv()
{
	if (IsConnected() == false) return;

	_recvEvent.Init();
	_recvEvent.SetOwner(shared_from_this());

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, &_recvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvEvent.SetOwner(nullptr); // RELEASE_REF
		}
	}

}

void Session::ProcessConnect()
{
	_connectEvent.SetOwner(nullptr);
	_connected.store(true);

	OnConnected();

	RegisterRecv();
	
}
void Session::ProcessDisconnect()
{
	_disconnectEvent.SetOwner(nullptr);
}
void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.SetOwner(nullptr);

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	

	RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes)
{
	_sendEvent.SetOwner(nullptr);
	_sendEvent.GetBuffer().clear();

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	OnSend(numOfBytes);

	RegisterSend();
}

void Session::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// TODO : Log
		cout << "Handle Error : " << errorCode << endl;
		break;
	}
}