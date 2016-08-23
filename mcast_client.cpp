#include "stddef.h"
#include "rw_buffer.h"
#include "multicast_client.h"
#include "time_handler.h"
#include "statistics_handler.h"
#include "config.h"

#define RWBUFF_LOW      1

static int OpenFile(const string& str, int openFlag)
{
    int     fd  = -1;
    if (!access(str.c_str(), 0))
    {
        fd  = open(str.c_str(), openFlag);
    }
    else
    {
        int creatFlag = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
        fd  = creat(str.c_str(), creatFlag);
    }
    return fd;
}

int main()
{
    Config  cfg("client.cfg");
    ClientCfg       clientCfg   = cfg.ReadClientCfg();
    cout << "mcast ip: " << clientCfg.ip << endl;
    cout << "mcast port: " << clientCfg.port << endl;
    cout << "recv buff size: " << clientCfg.recvBuffSize << endl;
    cout << "recv cache size: " << clientCfg.recvCacheSize << endl;
    cout << "rw buff low: " << clientCfg.rwBuffLow << endl;
    cout << "delay file: " << clientCfg.delayFile << endl;
    cout << "loss or disorder file: " << clientCfg.lossOrDisorderFile << endl;
    
    int delayLogFd;
    int lossOrDisorderFd;
    int openFlag    = O_WRONLY | O_APPEND;
    if (clientCfg.delayFile == "0")
    {
        delayLogFd      = 0;
    }
    else
    {
        delayLogFd = OpenFile(clientCfg.delayFile, openFlag);
        if (delayLogFd == -1)
        {
            cout << "open delay log file error: " << strerror(errno) << endl;
            return -1;
        }
    }
    if (clientCfg.lossOrDisorderFile == "0")
    {
        lossOrDisorderFd    = 0;
    }
    else
    {
        lossOrDisorderFd = OpenFile(clientCfg.lossOrDisorderFile, openFlag);
        if (lossOrDisorderFd == -1)
        {
            cout << "open loss_or_disorder log file error: " << strerror(errno) << endl;
            return -1;
        }
    }
    
    MulticastClient     mc(clientCfg.recvBuffSize);
    StatisticsHandler   ssHandler(clientCfg.rwBuffLow, delayLogFd, lossOrDisorderFd);
    pthread_t           tidStatistics;
    pthread_attr_t      tAttr;
    
    if (mc.Init(clientCfg.port) < 0)
    {
        cout << "init client error" << endl;
        return -1;
    }
    if (mc.SetRecvBuffSize(clientCfg.recvCacheSize) < 0)
    {
        cout << "set recv buff error" << endl;
        return -1;
    }
    if (mc.JoinGroup(clientCfg.ip) < 0)
    {
        cout << "join group error" << endl;
        return -1;
    }
    if (ssHandler.Init() < 0)
    {
        cout << "ss handler init error" << endl;
        return -1;
    }
    
    pthread_attr_init(&tAttr);
    pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setscope(&tAttr, PTHREAD_SCOPE_SYSTEM);
    if (TimeHandler::StartTick(NULL, &tAttr) < 0)
    {
        cout << "start time thread error" << endl;
        return -1;
    }
    while (true)
    {
        Msg*    pMsg    = mc.RecvMsg();
        if (pMsg == NULL)
        {
            cout << "recvmsg error" << endl;
            break;
        }
        ssHandler.HandleMsg(pMsg);
        delete pMsg;
    }
    pthread_attr_destroy(&tAttr);
    
    return 0;
}

/*
int main()
{
    int sockfd = 0;
    int rc = 0;
    sockaddr_in addr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd > 0);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(MCAST_PORT);

    int reuse = 1;
    rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    int mcastLoop = 0;
    rc = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &mcastLoop, sizeof(mcastLoop));
    assert(rc == 0);

    rc = bind(sockfd, (sockaddr*)&addr, sizeof(addr));
    assert(rc == 0);

    ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    rc = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    assert(rc == 0);

    const int BUFF_SIZE = 1024;
    char recvBuff[BUFF_SIZE] = { 0 };
    while (true)
    {
        socklen_t addrLen = sizeof(addr);
        memset(recvBuff, 0, sizeof(recvBuff));
        int n = recvfrom(sockfd, recvBuff, BUFF_SIZE, 0, (sockaddr*)&addr, &addrLen);
        if (n > 0)
        {
            recvBuff[n] = 0;
            cout << recvBuff << endl;
        }
        else
        {
            cout << "errno: " << errno << ", error str: " << strerror(errno) << endl;
        }

    }
    close(sockfd);

    return 0;
}
*/