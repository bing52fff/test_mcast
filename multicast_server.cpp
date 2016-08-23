#include "multicast_server.h"

MulticastServer::MulticastServer()
{
    m_sockFd        = -1;
    m_ipAddr        = "";
    m_port          = 0;
}

MulticastServer::~MulticastServer()
{
    if (m_sockFd != -1)
    {
        close(m_sockFd);
    }
}

int MulticastServer::Init(const string& strIp, uint16_t uiPort)
{
    if (strIp.empty() || uiPort <= 0)
    {
        return -1;
    }
    m_ipAddr        = strIp;
    m_port          = uiPort;
    m_mcastAddr     = GetSockaddrIn(m_ipAddr, m_port);
    m_sockFd        = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sockFd < 0)
    {
        return -1;
    }
    return 0;
}

int MulticastServer::SetSendBuffSize(int nBuffSize)
{
    return setsockopt(m_sockFd, SOL_SOCKET, SO_SNDBUF, &nBuffSize, sizeof(nBuffSize));
}

int MulticastServer::Send(const char* strSend, int64_t nLen, int flag)
{
    return sendto(m_sockFd, strSend, nLen, flag, (sockaddr*)&m_mcastAddr, sizeof(m_mcastAddr));
}

int MulticastServer::SendMsg(Msg& msgSend, int flag)
{
    return Send(msgSend.ToString(), msgSend.uLen, flag);
}
