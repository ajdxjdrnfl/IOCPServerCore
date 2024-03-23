#include "pch.h"
#include "Session.h"
#include "SendBuffer.h"
#include "RecvBuffer.h"
#include "RawBuffer.h"
#include "Service.h"

Session::Session() : _recvBuffer(RECV_BUFFER_SIZE)
{
	
}

Session::~Session()
{

}

void Session::Send(SendBufferRef sendBuffer)
{
	
	bool registered = false;
	{
		PushSendBuffer(sendBuffer);
		registered = _sendRegistered.exchange(true) == false;
	}
	if (registered)
	{
		RegisterSend();
	}
	
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const wstring& cause)
{

	if (_connected.exchange(false) == false)
		return;

	wcout << "Disconnect : " << cause << endl;

	OnDisconnected();
	GetService()->ReleaseSession(GetSessionRef());

	RegisterDisconnect();
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
		ProcessRecv(numOfBytes);
		break;
	case IocpEventType::Send:
		ProcessSend(numOfBytes);
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

	IocpEvent* event = new IocpEvent(IocpEventType::Disconnect);
	event->SetOwner(shared_from_this());
	AddIocpEvent(event);
	
	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (false == SocketManager::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, event))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			RemoveIocpEvent(event);
			return false;
		}
	}
	return true;
	
}

bool Session::RegisterDisconnect()
{
	if (!IsConnected()) return false;

	IocpEvent* event = new IocpEvent(IocpEventType::Disconnect);
	event->SetOwner(shared_from_this());
	AddIocpEvent(event);

	if (false == SocketManager::DisconnectEx(_socket, event, TF_REUSE_SOCKET, 0))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			RemoveIocpEvent(event);
			return false;
		}
	}

	return true;
}
void Session::RegisterSend()
{
	if (IsConnected() == false)
		return;

	IocpEvent* event = new IocpEvent(IocpEventType::Send);
	event->SetOwner(shared_from_this());
	event->Init();
	AddIocpEvent(event);

	PopAllSendBuffer(event);

	vector<WSABUF> wsaBufs;

	vector<WSABUF> wsaBufs;
	wsaBufs.reserve(event->GetBufferLength());

	for (RawBufferRef buffer : event->GetBuffers())
	{
		WSABUF wsaBuf;
		SendBufferRef sendBuffer = static_pointer_cast<SendBuffer>(buffer);
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->GetRawBuffer());
		wsaBuf.len = (ULONG)(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD sendLen = 0;
	DWORD flags = 0;

	DWORD numOfBytes = 0;
	if (::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), &numOfBytes, 0, event, nullptr) == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			RemoveIocpEvent(event);
		}
	}
}
void Session::RegisterRecv()
{
	if (IsConnected() == false) return;

	IocpEvent* event = new IocpEvent(IocpEventType::Recv);
	event->SetOwner(shared_from_this());
	event->Init();
	AddIocpEvent(event);

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, event, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			RemoveIocpEvent(event);
		}
	}

}

void Session::ProcessConnect()
{
	
	_connected.store(true);

	// 세션 등록
	GetService()->AddSession(GetSessionRef());

	OnConnected();

	RegisterRecv();
	
}

void Session::ProcessDisconnect()
{
	
}

void Session::ProcessRecv(int32 numOfBytes)
{
	

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	int32 dataSize = _recvBuffer.DataSize();

	int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
	
	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}

	_recvBuffer.Clean();

	RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes)
{
	
	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	OnSend(numOfBytes);

	bool registered = false;
	{
		WRITE_LOCK(SEND);
		if (_sendQueue.empty())
			_sendRegistered.store(false);

		else
			registered = true;
	}
	if(registered)
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

void Session::RemoveIocpEvent(IocpEvent* iocpEvent)
{
	WRITE_LOCK(EVENT);

	auto it = remove(_iocpEvents.begin(), _iocpEvents.end(), iocpEvent);
	_iocpEvents.erase(it, _iocpEvents.end());

	SAFE_DELETE(iocpEvent)
}

void Session::AddIocpEvent(IocpEvent* iocpEvent)
{
	WRITE_LOCK(Event);

	auto it = find(_iocpEvents.begin(), _iocpEvents.end(), iocpEvent);
	if (it == _iocpEvents.end())
		return;

	_iocpEvents.push_back(iocpEvent);

}

void Session::PushSendBuffer(SendBufferRef sendBuffer)
{
	WRITE_LOCK(SEND);

	_sendQueue.push(sendBuffer);
	
}

void Session::PopAllSendBuffer(IocpEvent* iocpEvent)
{
	WRITE_LOCK(SEND);

	int writeSize = 0;
	while (!_sendQueue.empty())
	{
		SendBufferRef sendBuffer = _sendQueue.front();
		_sendQueue.pop();

		writeSize += sendBuffer->WriteSize();
		iocpEvent->AddBuffer(sendBuffer);
	}

	if (writeSize >= SEND_BUFFER_SIZE)
	{
		// TODO : Send Buffer Overflow 에러
	}
}
