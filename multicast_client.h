#ifndef _MULTICAST_CLIENT_H
#define _MULTICAST_CLIENT_H

#include "stddef.h"
#include "data_struct.h"
#include "multicast_base.h"

class MulticastClient : MulticastBase
{
protected:
    enum { MAX_BUFF_SIZE = (1 << 16) };
public:
    MulticastClient(int nBuffSize = MAX_BUFF_SIZE);
    ~MulticastClient();
    
    int SetRecvBuffSize(int nBuffSize);
    
    int GetRecvBuffSize();
    
    int Init(uint16_t port, int reuse = 1);
    
    int JoinGroup(const string& strGrpIp);
    
    Msg* RecvMsg(int flag = 0);
    
private:
    sockaddr_in         m_sockAddr;
    char*               m_pRecvBuff;
    int                 m_nBuffSize;
    string              m_groupIp;
    uint16_t            m_groupPort;
};

#endif /* _MULTICAST_CLIENT_H */

