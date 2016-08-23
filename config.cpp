#include "config.h"

Config::Config(const string& cfgFilename)
{
    if (cfgFilename.empty())
    {
        return ;
    }
    vector<string>      lines;
    ifstream            file(cfgFilename.c_str());
    string              line;
    while (!file.eof())
    {
        getline(file, line);
        if (!line.empty())
        {
            lines.push_back(line);
        }
    }
    for (vector<string>::iterator iter = lines.begin(); iter != lines.end(); iter ++)
    {
        if ((*iter)[0] == '#')
        {
            continue;
        }
        int pos         = (*iter).find('=');
        if (pos == -1)
        {
            continue ;
        }
        string key      = Config::Trim(iter->substr(0, pos));
        string value    = Config::Trim(iter->substr(pos + 1, iter->size() - pos - 1));
        if (!key.empty() && !value.empty())
        {
            m_cfgMap[key]   = value;
        }
    }
}
    
string Config::GetValue(string key)
{
    return m_cfgMap[key];
}
    
void Config::AddValue(string key, string value)
{
    m_cfgMap[key]       = value;
}

string Config::Trim(const string& str)
{
    int     leftIndex   = 0;
    int     rightIndex  = str.size() - 1;
    for (; leftIndex < str.size(); leftIndex ++)
    {
        if (isspace(str[leftIndex]))
        {
            continue;
        }
        break;
    }
    if (leftIndex >= str.size())
    {
        return "";
    }
    for (; rightIndex > leftIndex; rightIndex --)
    {
        if (isspace(str[rightIndex]))
        {
            continue;
        }
        break;
    }
    return str.substr(leftIndex, rightIndex - leftIndex + 1);
}

ClientCfg Config::ReadClientCfg()
{
    string  mcastIp     = GetValue("mcast_ip");
    if (mcastIp.empty())
    {
        mcastIp         = MCAST_ADDR;
    }
    string  mcastPort   = GetValue("mcast_port");
    if (mcastPort.empty())
    {
        mcastPort       = MCAST_PORT;
    }
    string  recvSize    = GetValue("recv_cache_size");
    if (recvSize.empty())
    {
        recvSize        = RECV_CACHE_SIZE;
    }
    string buffSize     = GetValue("recv_buff_size");
    if (buffSize.empty())
    {
        buffSize        = RECV_BUFF_SIZE;
    }
    string rwBuffLow    = GetValue("rw_buff_low");
    if (rwBuffLow.empty())
    {
        rwBuffLow       = RW_BUFF_LOW;
    }
    string delayFile    = GetValue("delay_file");
    if (delayFile.empty())
    {
        delayFile       = DELAY_FILE;
    }
    string lossFile     = GetValue("loss_or_disorder_file");
    if (lossFile.empty())
    {
        lossFile        = LOSS_OR_DISORDER_FILE;
    }
    ClientCfg               clientCfg;
    clientCfg.ip            = mcastIp;
    clientCfg.delayFile     = delayFile;
    clientCfg.port          = atol(mcastPort.c_str());
    clientCfg.recvBuffSize  = atol(buffSize.c_str());
    clientCfg.recvBuffSize  = 1 << clientCfg.recvBuffSize;
    clientCfg.recvCacheSize = atol(recvSize.c_str());
    clientCfg.recvCacheSize = 1 << clientCfg.recvCacheSize;
    clientCfg.rwBuffLow     = atol(rwBuffLow.c_str());
    clientCfg.lossOrDisorderFile    = lossFile;
    return clientCfg;
}

ServerCfg Config::ReadServerCgf()
{
    string  mcastIp     = GetValue("mcast_ip");
    if (mcastIp.empty())
    {
        mcastIp         = MCAST_ADDR;
    }
    string  port        = GetValue("mcast_port");
    if (port.empty())
    {
        port            = MCAST_PORT;
    }
    string  sendCache   = GetValue("send_cache_size");
    if (sendCache.empty())
    {
        sendCache       = SEND_CACHE_SIZE;
    }
    string  sendInter   = GetValue("send_interval");
    if (sendInter.empty())
    {
        sendInter       = SEND_INTERVAL;
    }
    string  countPerGap = GetValue("count_per_gap");
    if (countPerGap.empty())
    {
        countPerGap     = COUNT_PER_GAP;
    }
    string  packageSize = GetValue("package_size");
    if (packageSize.empty())
    {
        packageSize     = PACKAGE_SIZE;
    }
    string  stPerSec    = GetValue("settime_per_sec");
    if (stPerSec.empty())
    {
        stPerSec        = SETTIME_PER_SEC;
    }
    ServerCfg               svrCfg;
    svrCfg.ip               = mcastIp;
    svrCfg.port             = atol(port.c_str());
    svrCfg.sendCacheSize    = atol(sendCache.c_str());
    svrCfg.sendCacheSize    = 1 << svrCfg.sendCacheSize;
    svrCfg.sendInterval     = atol(sendInter.c_str());
    svrCfg.countPerGap      = atol(countPerGap.c_str());
    svrCfg.packageSize      = atol(packageSize.c_str());
    svrCfg.settimePerSec    = atol(stPerSec.c_str());
    return svrCfg;
}