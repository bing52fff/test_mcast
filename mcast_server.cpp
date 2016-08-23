#include "stddef.h"
#include "config.h"
#include "time_handler.h"
#include "multicast_server.h"

int main()
{
    Config          cfg("server.cfg");
    ServerCfg       serverCfg   = cfg.ReadServerCgf();
    
    cout << "mcast ip: " << serverCfg.ip << endl;
    cout << "mcast port: " << serverCfg.port << endl;
    cout << "send cache size: " << serverCfg.sendCacheSize << endl;
    cout << "send interval: " << serverCfg.sendInterval << endl;
    cout << "count per gap: " << serverCfg.countPerGap << endl;
    cout << "package size: " << serverCfg.packageSize << endl;
    cout << "settime per sec: " << serverCfg.settimePerSec << endl;
    
    MulticastServer ms;
    ms.Init(serverCfg.ip, serverCfg.port);
    ms.SetSendBuffSize(serverCfg.sendCacheSize);
    Msg msg(1, serverCfg.packageSize);
    char *pSendBuff = new char[msg.uLen];
    int len = msg.uLen;
    
    if (TimeHandler::StartTick() < 0)
    {
        cout << "start time thread error" << endl;
        return -1;
    }
    
    int nByteSend       = 0;
    int64_t timeToSleep = 0;
    int64_t interval    = serverCfg.sendInterval;
    int countPerGap     = serverCfg.countPerGap;
    int countPerSec     = (1000 * countPerGap) / (interval);
    int settimePerSec   = serverCfg.settimePerSec;
    if (countPerSec < settimePerSec)
    {
        settimePerSec   = countPerSec;
    }
    int settimeInterval = countPerSec / settimePerSec;
    int64_t preTime     = TimeHandler::GetCurrentTime();
    while (true)
    {
        for (int ix = 0; ix < countPerGap; ix ++)
        {
            msg.UpdateSendTime(msg.id % settimeInterval == 0);
            nByteSend = ms.Send(msg.ToString(pSendBuff, len), len, 0);
            if (nByteSend == msg.uLen)
            {
                msg.IncId();
            }
            else if (nByteSend >= 0)
            {
                cout << "package don't send completely" << endl;
            }
            else if (errno != EINTR)
            {
                cout << "send package error, errno: " << errno << endl;
            }
        }
        preTime += interval;
        timeToSleep = preTime - TimeHandler::GetCurrentTime();
        if (timeToSleep > 0)
        {
            usleep(timeToSleep * 1000);
        }
    }
    return 0;
}

/*
int main()
{
    int sockfd = 0;
    sockaddr_in mcastAddr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd > 0);
    memset(&mcastAddr, 0, sizeof(mcastAddr));
    mcastAddr.sin_family = AF_INET;
    mcastAddr.sin_addr.s_addr = inet_addr(MCAST_ADDR);
    mcastAddr.sin_port = htons(MCAST_PORT);

    Msg msg;
    msg.SetId(1);
    msg.UpdateSendTime();
    while (true)
    {
        char* strSend = msg.ToString((1 << 16) -1 - 20 - 8); //(1<<16)-1 max_udp_total_len; 20 ip_header_len; 8 udp_header_len;
        int n = sendto(sockfd, strSend, msg.uLen, 0, (struct sockaddr*)&mcastAddr, sizeof(mcastAddr));
        if (n > 0)
        {
            msg.IncId();
            msg.UpdateSendTime();
            delete []strSend;
            sleep(5);
        }
        else
        {
            cout << "errno: " << errno << ", error str: " << strerror(errno) << endl;
        }
    }
    return 0;
}
*/
