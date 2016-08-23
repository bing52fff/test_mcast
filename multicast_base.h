#ifndef _MULTICAST_BASE_H
#define _MULTICAST_BASE_H

#include "stddef.h"

class MulticastBase
{
public:
    sockaddr_in GetSockaddrIn(const string& strIp, uint16_t port);
    
    sockaddr_in GetSockaddrIn(in_addr_t ip, uint16_t port);
    
    sockaddr_in _GetSockaddrIn(in_addr_t ip, uint16_t port);
    
protected:
    int         m_sockFd;
};

#endif /* _MULTICAST_BASE_H */

