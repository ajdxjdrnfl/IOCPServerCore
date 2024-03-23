#include "pch.h"
#include "NetAddress.h"

NetAddress::NetAddress(SOCKADDR_IN address) : _address(address)
{
	
}

NetAddress::NetAddress(wstring ip, uint16 port, int32 family)
{
	::memset(&_address, 0, sizeof(_address));
	_address.sin_family = family;
	_address.sin_port = htons(port);
	_address.sin_addr = IpToAddress(ip.data(), family);
	
}

wstring NetAddress::GetIpAddress()
{
	WCHAR buffer[100];
	::InetNtopW(_address.sin_family, &_address.sin_addr, buffer, static_cast<int32>(sizeof(buffer) / sizeof(buffer[0])));
	return wstring(buffer);
}

IN_ADDR NetAddress::IpToAddress(const WCHAR* ip, int32 family)
{
	IN_ADDR ipAddress;
	::InetPtonW(family, ip, &ipAddress);
	return ipAddress;
}

