#include "stddef.h"
#include "multicast_client.h"
#include "time_handler.h"

MulticastClient::MulticastClient(int nBuffSize)
{
    m_sockFd        = -1;
    m_nBuffSize     = nBuffSize;
    m_pRecvBuff     = new char[m_nBuffSize];
}

MulticastClient::~MulticastClient()
{
    if (m_sockFd != -1)
    {
        close(m_sockFd);
    }
}

int MulticastClient::SetRecvBuffSize(int nBuffSize)
{
    return setsockopt(m_sockFd, SOL_SOCKET, SO_RCVBUF, &nBuffSize, sizeof(nBuffSize));
}

int MulticastClient::GetRecvBuffSize()
{
    int buffSize = 0;
    socklen_t len = sizeof(buffSize);
    getsockopt(m_sockFd, SOL_SOCKET, SO_RCVBUF, &buffSize, &len);
    return buffSize;
}

int MulticastClient::Init(uint16_t port, int reuse)
{
    int     rc      = 0;
    m_groupPort     = port;
    m_sockAddr      = GetSockaddrIn(INADDR_ANY, port);
    m_sockFd        = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sockFd < 0)
    {
        return -1;
    }
    if (reuse  == 1)
    {
        rc  = setsockopt(m_sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        if (rc != 0)
        {
            return -1;
        }
    }
    rc  = bind(m_sockFd, (sockaddr*)&m_sockAddr, sizeof(m_sockAddr));
    if (rc < 0)
    {
        return -1;
    }
    return 0;
}

int MulticastClient::JoinGroup(const string& strGrpIp)
{
    ip_mreq         mreq;
    int     rc      = 0;
    m_groupIp       = strGrpIp;
    mreq.imr_interface.s_addr   = htonl(INADDR_ANY);
    mreq.imr_multiaddr.s_addr   = inet_addr(m_groupIp.c_str());
    rc      = setsockopt(m_sockFd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (rc != 0)
    {
        return -1;
    }
    return 0;
}

Msg* MulticastClient::RecvMsg(int flag)
{
    memset(m_pRecvBuff, 0, m_nBuffSize);
    socklen_t   addrLen     = sizeof(m_sockAddr);
    int nBytesRecv = recvfrom(m_sockFd, m_pRecvBuff, m_nBuffSize, flag, (sockaddr*)&m_sockAddr, &addrLen);
    if (nBytesRecv <= sizeof(Msg))
    {
        return NULL;
    }
    Msg*    pMsg    = new Msg(m_pRecvBuff);
    if (pMsg == NULL || pMsg->uLen != nBytesRecv)
    {
        return NULL;
    }
    pMsg->UpdateRecvTime();
    return pMsg;
}

