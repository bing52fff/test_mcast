#ifndef _RW_BUFFER_H
#define _RW_BUFFER_H

#include "stddef.h"
#include "data_struct.h"

template <typename T>
struct LinkNode
{
    T               tData;
    LinkNode*       m_pNext;
};

template <typename T>
struct HeadNode
{
    LinkNode<T>*    m_pFirst;
    LinkNode<T>*    m_pLast;
    int64_t         m_size;
};

template <typename T>
class RWBuffer
{
public:
    RWBuffer(int buffLow = 1)
    {
        pthread_mutex_init(&m_lock, NULL);
        m_pHead             = new HeadNode<T>;
        m_pHead->m_pFirst   = NULL;
        m_pHead->m_pLast    = NULL;
        m_pHead->m_size     = 0;
        m_buffLow           = buffLow;
    }
    ~RWBuffer()
    {
        pthread_mutex_destroy(&m_lock);
    }
    
    int EnQueue(const T& value)
    {
        LinkNode<T>*    pNode   = new LinkNode<T>;
        pNode->tData    = value;
        pNode->m_pNext  = NULL;
        pthread_mutex_lock(&m_lock);
        if (m_pHead->m_size == 0)
        {
            m_pHead->m_pFirst   = pNode;
            m_pHead->m_pLast    = pNode;
            m_pHead->m_size     ++;
        }
        else
        {
            m_pHead->m_pLast->m_pNext   = pNode;
            m_pHead->m_pLast            = pNode;
            m_pHead->m_size             ++;
        }
        pthread_mutex_unlock(&m_lock);
        return 0;
    }
    
    int DeQueue(LinkNode<T>*& pList)
    {
        if (m_pHead->m_size < m_buffLow)
        {
            return -1;
        }
        pthread_mutex_lock(&m_lock);
        pList                   = m_pHead->m_pFirst;
        m_pHead->m_pFirst       = NULL;
        m_pHead->m_pLast        = NULL;
        m_pHead->m_size         = 0;
        pthread_mutex_unlock(&m_lock);
        return 0;
    }
    
    void Foreach(LinkNode<T>* pList, void (*pFunc)(void*, const T&), void* arg)
    {
        LinkNode<T>*    pNode   = NULL;
        while (pList != NULL)
        {
            pNode       = pList;
            pList       = pList->m_pNext;
            pFunc(arg, pNode->tData);
            delete      pNode;
        }
    }
private:
    HeadNode<T>*        m_pHead;
    int                 m_buffLow;
    pthread_mutex_t     m_lock;
    /*
public:
    RWBuffer()
    {
        pthread_mutex_init(&m_lock, NULL);
    }
    ~RWBuffer()
    {
        pthread_mutex_destroy(&m_lock);
    }
    
    int EnQueue(Msg* pMsg)
    {
        pthread_mutex_lock(&m_lock);
        m_data.push_back(pMsg);
        pthread_mutex_unlock(&m_lock);
        return 0;
    }
    
    Msg* DeQueue()
    {
        Msg*        pMsg    = NULL;
        if (m_data.empty())
        {
            return pMsg;
        }
        pthread_mutex_lock(&m_lock);
        if (m_data.empty())
        {
            return pMsg;
        }
        pMsg        = m_data.front();
        m_data.pop_front();
        pthread_mutex_unlock(&m_lock);
        return pMsg;
    }
private:
    deque<Msg*>         m_data;
    pthread_mutex_t     m_lock;
    */
};

#endif /* _RW_BUFFER_H */

