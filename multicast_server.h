#ifndef _MULTICAST_SERVER_H
#define _MULTICAST_SERVER_H

#include "stddef.h"
#include "data_struct.h"
#include "multicast_base.h"

class MulticastServer : MulticastBase
{
public:
    MulticastServer();
    ~MulticastServer();
    
    int Init(const string& strIp, uint16_t uiPort);
    
    int SetSendBuffSize(int nBuffSize);
    
    int Send(const char* str, int64_t nLen, int flag = 0);
    
    int SendMsg(Msg& msgSend, int flag = 0);
    
private:
    sockaddr_in m_mcastAddr;
    string      m_ipAddr;
    uint16_t    m_port;
};

#endif /* _MULTICAST_SERVER_H */

