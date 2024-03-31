#include "pch.h"
#include "HttpSession.h"


HttpSession::HttpSession()
{


}

HttpSession::~HttpSession()
{

}

void HttpSession::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	
}

bool HttpSession::Connect()
{

}

void HttpSession::Disconnect()
{

}

int32 HttpSession::OnRecv(BYTE* buffer, int32 numOfBytes)
{
	int32 processLen = 0;
	
	{
		// TODO : Parsing start line, headers, body : http request
		
	}

}

void HttpSession::OnSend(int32 numOfBytes)
{
	if (_sendBuffer.WriteSize() == numOfBytes)
	{
		Disconnect();
	}
}