#include "statistics_handler.h"
#include "rw_buffer.h"

void* StatisticsHandler::HandleFunc(void* arg)
{
    /*RWBuffer*   pRwBuffer   = (RWBuffer*) arg;
    Msg*        pMsg        = NULL;
    StatisticsHandler       ssHandler;
    while (true)
    {
        pMsg    = pRwBuffer->DeQueue();
        if (pMsg == NULL)
        {
            usleep(500);
            continue;
        }
        ssHandler.HandleMsg(pMsg);
        delete  pMsg;
        pMsg    = NULL;
    }*/
    return NULL;
}

void StatisticsHandler::WriteToFile(void* arg, const WriteNotifyNode& node)
{
    StatisticsHandler*      pSsHandler  = (StatisticsHandler*) arg;
    switch (node.type)
    {
    case StatisticsHandler::DELAY:
        pSsHandler->WriteDelay(node.value);
        break;
    case StatisticsHandler::LOSS:
    case StatisticsHandler::DISORDER:
        pSsHandler->WriteLossOrDisorder(node.value);
        break;
    default:
        break;
    }
}

void* StatisticsHandler::StartWrite(void* arg)
{
    StatisticsHandler*          pSsHandler  = (StatisticsHandler*) arg;
    LinkNode<WriteNotifyNode>*  pList   = NULL;
    while (true)
    {
        if (pSsHandler->m_rwBuffer.DeQueue(pList) < 0)
        {
            //数据不够 睡眠100ms
            usleep(100 * 1000);
        }
        else
        {
            pSsHandler->m_rwBuffer.Foreach(pList, &StatisticsHandler::WriteToFile, pSsHandler);
        }
    }
    return NULL;
}

StatisticsHandler::StatisticsHandler(int rwBuffLow, int delayFd, int lossOrDisorderFd, int64_t msDelayInterval)
    : m_rwBuffer(rwBuffLow)
{
    m_delayFd           = delayFd;
    m_lossOrDisorderFd  = lossOrDisorderFd;
    m_msDelayInterval   = msDelayInterval;
    m_isFirstPackage    = true;
    m_isDelayLoopBegin  = true;
}

int StatisticsHandler::Init()
{
    pthread_t           tid;
    pthread_attr_t      tAttr;
    pthread_attr_init(&tAttr);
    pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setscope(&tAttr, PTHREAD_SCOPE_SYSTEM);
    int rc      = pthread_create(&tid, &tAttr, &StatisticsHandler::StartWrite, this);
    if (rc != 0)
    {
        return -1;
    }
    return 0;
}

void StatisticsHandler::HandleMsg(Msg* pMsg)
{
    HandleDelay(pMsg);
    HandleLossOrDisorder(pMsg);
    if (IsFirstPackage())
    {
        m_isFirstPackage    = false;
    }
}

void StatisticsHandler::HandleDelay(Msg* pMsg)
{
    if (!pMsg->isSetTime)
    {
        return ;
    }
    stringstream    ss;
    WriteNotifyNode node;
    int64_t         delay   = pMsg->tRecv - pMsg->tSend;
    ss << pMsg->id << "|" << delay << "|" << pMsg->uLen << "|" \
       << pMsg->tSend << "|" << pMsg->tRecv << endl;
    node.type       = StatisticsHandler::DELAY;
    node.value      = ss.str();
    m_rwBuffer.EnQueue(node);
    /*if (IsDelayLoopBegin())
    {
        m_preDelayTime      = pMsg->tRecv;
        m_minDelay          = delay;
        m_maxDelay          = delay;
        m_aveDelay          = delay;
        m_packageCount      = 1;
        m_isDelayLoopBegin  = false;
    }
    else
    {
        m_minDelay          = delay < m_minDelay ? delay : m_minDelay;
        m_maxDelay          = delay > m_maxDelay ? delay : m_maxDelay;
        m_aveDelay          += delay;
        m_packageCount      ++;
        if (pMsg->tRecv - m_preDelayTime >= m_msDelayInterval)
        {
            m_aveDelay      /= m_packageCount;
            ss << m_minDelay << "|" << m_maxDelay << "|" << m_aveDelay << "|" \
               << m_packageCount << "|" << m_preDelayTime << "," << pMsg->tRecv << endl;
            outStr              = ss.str();
            write(m_delayFd, outStr.c_str(), outStr.size());
            m_preDelayTime      = pMsg->tRecv;
            m_isDelayLoopBegin  = true;
        }
    }*/
}
    
void StatisticsHandler::HandleLossOrDisorder(Msg* pMsg)
{
    if (IsFirstPackage())
    {
        m_prePackageId  = pMsg->id;
        return ;
    }
    uint64_t    curId   = pMsg->id;
    if (curId == m_prePackageId + 1)
    {
        m_prePackageId  = curId;
        return ;
    }
    else 
    {
        stringstream    ss;
        WriteNotifyNode node;
        if (curId > m_prePackageId + 1)
        {
            ss << StatisticsHandler::LOSS << "|";
            for (uint64_t ix = m_prePackageId + 1; ix < curId; ix ++)
            {
                ss << ix << ",";
            }
            ss << "|" << pMsg->tRecv << endl;
            m_prePackageId  = curId;
            node.type       = StatisticsHandler::LOSS;
            node.value      = ss.str();
            m_rwBuffer.EnQueue(node);
        }
        else if (curId < m_prePackageId + 1)
        {
            ss << StatisticsHandler::DISORDER << "|";
            ss << curId << "|";
            ss << pMsg->tRecv << endl;
            node.type       = StatisticsHandler::DISORDER;
            node.value      = ss.str();
            m_rwBuffer.EnQueue(node);
        }
    }
}

void StatisticsHandler::WriteDelay(const string& str)
{
    write(m_delayFd, str.c_str(), str.size());
}
    
void StatisticsHandler::WriteLossOrDisorder(const string& str)
{
    write(m_lossOrDisorderFd, str.c_str(), str.size());
}