#include "multicast_base.h"

sockaddr_in MulticastBase::GetSockaddrIn(const string& strIp, uint16_t port)
{
    return _GetSockaddrIn(inet_addr(strIp.c_str()), port);
}

sockaddr_in MulticastBase::GetSockaddrIn(in_addr_t ip, uint16_t port)
{
    return _GetSockaddrIn(htonl(ip), port);
}

sockaddr_in MulticastBase::_GetSockaddrIn(in_addr_t ip, uint16_t port)
{
    sockaddr_in             addr;
    addr.sin_family         = AF_INET;
    addr.sin_addr.s_addr    = ip;
    addr.sin_port           = htons(port);
    return addr;
}

