#ifndef _STATISTICS_HANDLER_H
#define _STATISTICS_HANDLER_H

#include "stddef.h"
#include "data_struct.h"
#include "rw_buffer.h"

struct WriteNotifyNode
{
    int         type;
    string      value;
};

class StatisticsHandler
{
public:
    enum { NORMAL = 0, DELAY = 1, LOSS = 2, DISORDER = 3 };
    
    static void* HandleFunc(void* arg);
    
    static void* StartWrite(void* arg);
    
    static void WriteToFile(void* arg, const WriteNotifyNode& node);
    
public:
    StatisticsHandler(int rwBuffLow = 1, int delayFd = 0, int lossOrDisorderFd = 0, int64_t msDelayInterval = 2000);
    
    int Init();
    
    void HandleMsg(Msg* pMsg);
    
    void HandleDelay(Msg* pMsg);
    
    void HandleLossOrDisorder(Msg* pMsg);
    
    void WriteDelay(const string& str);
    
    void WriteLossOrDisorder(const string& str);
    
protected:
    
    bool IsFirstPackage() { return m_isFirstPackage; }
    
    bool IsDelayLoopBegin() { return m_isDelayLoopBegin; }
    
private:
    RWBuffer<WriteNotifyNode>    m_rwBuffer;
    
    int         m_delayFd;
    bool        m_isDelayLoopBegin;
    int64_t     m_msDelayInterval;
    int64_t     m_packageCount;
    int64_t     m_maxDelay;
    int64_t     m_minDelay;
    int64_t     m_aveDelay;
    int64_t     m_preDelayTime;
    
    int         m_lossOrDisorderFd;
    bool        m_isFirstPackage;
    uint64_t    m_prePackageId;
};

#endif /* _STATISTICS_HANDLER_H */

