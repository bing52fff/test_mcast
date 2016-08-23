#ifndef _TIME_HANDLER_H
#define _TIME_HANDLER_H

#include "stddef.h"

class TimeHandler
{
public:
    static int StartTick(void* arg = NULL, pthread_attr_t *attr = NULL);
    
    static int64_t GetCurrentTime(bool isReal = true)
    {
        if (isReal)
        {
            timeval tvCur;
            gettimeofday(&tvCur, NULL);
            return tvCur.tv_sec * 1000 * 1000 + tvCur.tv_usec;
        }
        else 
        {
            return g_curTime;
        }
    }
    
    static void* UpdateTime(void* arg);
    
    static int64_t      g_curTime;
    static bool         g_isReady;
};


#endif /* _TIME_HANDLER_H */

