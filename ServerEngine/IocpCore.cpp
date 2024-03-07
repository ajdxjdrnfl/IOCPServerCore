#include "pch.h"
#include "IocpCore.h"

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	assert(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(SOCKET socket)
{
	return ::CreateIoCompletionPort((HANDLE)socket, _iocpHandle, 0, 0 );

}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key;

	if (::GetQueuedCompletionStatus(_iocpHandle, &numOfBytes, reinterpret_cast<PULONG_PTR>(&key), , timeoutMs))
	{

	}
	else
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO : ·Î±×
			break;
		}
	}
}