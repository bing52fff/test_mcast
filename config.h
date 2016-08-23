#ifndef _CONFIG_H
#define _CONFIG_H

#include "stddef.h"

#define MCAST_ADDR      "239.0.0.1"
#define MCAST_PORT      "20202"
#define RECV_BUFF_SIZE  "16"
#define RECV_CACHE_SIZE "27"
#define RW_BUFF_LOW     "1"
#define DELAY_FILE      "delay.log"
#define LOSS_OR_DISORDER_FILE   "loss_or_disorder.log"

#define SEND_CACHE_SIZE "27"
#define SEND_INTERVAL   "100"
#define COUNT_PER_GAP   "10"
#define PACKAGE_SIZE    "1024"
#define SETTIME_PER_SEC "5"

struct ClientCfg
{
    string      ip;
    uint16_t    port;
    int32_t     recvBuffSize;
    int32_t     recvCacheSize;
    int32_t     rwBuffLow;
    string      delayFile;
    string      lossOrDisorderFile;
};

struct ServerCfg
{
    string      ip;
    uint16_t    port;
    int32_t     sendCacheSize;
    int32_t     sendInterval;
    int32_t     countPerGap;
    int32_t     packageSize;
    int32_t     settimePerSec;
};

class Config
{
public:
    Config(const string& cfgFilename);
    
    string GetValue(string key);
    
    void AddValue(string key, string value);
    
    ClientCfg ReadClientCfg();
    
    ServerCfg ReadServerCgf();
    
public:
    static string Trim(const string& str);
    
private:
    map<string, string>     m_cfgMap;
};

#endif /* _CONFIG_H */

