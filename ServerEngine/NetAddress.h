#pragma once
class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN address);
	NetAddress(wstring ip, uint16 port, int32 family = AF_INET);

	SOCKADDR_IN GetSockAddr() { return _address; }
	wstring GetIpAddress();
	IN_ADDR IpToAddress(const WCHAR* ip, int32 family);

private:
	SOCKADDR_IN _address = { };
};

