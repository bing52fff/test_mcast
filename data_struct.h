#ifndef __DATA_STRUCT_H
#define __DATA_STRUCT_H

#include "stddef.h"
#include "time_handler.h"

struct Msg
{
    uint64_t    id;
    int64_t     uLen;
    uint64_t    tSend;
    uint64_t    tRecv;
    bool        isSetTime;
    char*       padding;

    Msg()
    {
        this->id        = 0;
        this->uLen      = -1;
        this->tSend     = 0;
        this->tRecv     = 0;
        this->isSetTime = false;
        this->padding   = 0;
    }
    
    Msg(int64_t id, int64_t packageLen)
    {
        this->id        = id;
        this->uLen      = packageLen;
        this->tSend     = 0;
        this->tRecv     = 0;
        this->isSetTime = false;
        this->padding   = 0;
    }
    
    Msg(const char* pStrMsg, bool needPadding = false)
    {
        Msg *pMsg       = (Msg*) pStrMsg;
        this->id        = pMsg->id;
        this->uLen      = pMsg->uLen;
        this->tSend     = pMsg->tSend;
        this->tRecv     = pMsg->tRecv;
        this->isSetTime = pMsg->isSetTime;
        this->padding   = 0;
        if (needPadding)
        {
            int paddingLen  = pMsg->uLen - sizeof(Msg);
            this->padding   = new char[paddingLen];
            strncpy(this->padding, pStrMsg + sizeof(Msg), paddingLen);
        }
    }

    void SetSendTime(uint64_t t) { this->tSend = t; }
    
    void SetRecvTime(uint64_t t) { this->tRecv = t; }
    
    void SetId(uint64_t id) { this->id = id; }
    
    void IncId() { this->id ++; }
    
    void UpdateSendTime(bool needRealTime)
    {
        this->tSend         = TimeHandler::GetCurrentTime(needRealTime);
        this->isSetTime     = needRealTime;
    }
    
    void UpdateRecvTime()
    {
        this->tRecv = TimeHandler::GetCurrentTime(isSetTime);
    }

    char* ToString(char*& pBuff, int &nLen)
    {
        if (pBuff == NULL || nLen < uLen)
        {
            pBuff   = new char[uLen];
            nLen    = uLen;
        }
        memmove(pBuff, this, sizeof(Msg));
        this->padding = pBuff + sizeof(Msg);
        //padding with random bytes
        //memset(this->padding, 'a', nLen - sizeof(Msg));
        return pBuff;
    }
    
    char* ToString()
    {
        if (uLen <= sizeof(struct Msg))
        {
            return NULL;
        }
        char*   pData = new char[uLen];
        this->padding = pData + sizeof(struct Msg);
        memmove(pData, this, sizeof(struct Msg));
        memset(pData + sizeof(struct Msg), 'a', uLen - sizeof(struct Msg));
        return pData;
    }

    static struct Msg* Parse(const char * str)
    {
        return (struct Msg*) str;
    }

    int WriteToFile(int fd, bool isWritePadding = false)
    {
        if (fd < 0)
        {
            return -1;
        }
        stringstream ss;
        ss << id << "|";
        ss << uLen << "|";
        ss << tSend << "|";
        ss << tRecv << "|";
        if (isWritePadding)
        {
            ss << padding << endl;
        }
        else
        {
            ss << endl;
        }
        string str = ss.str();
        write(fd, str.c_str(), str.size());
        return 0;
    }

private:
    /*uint64_t _CurrentTime()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }
    */
};

#endif  //DATA_STRUCT_H
