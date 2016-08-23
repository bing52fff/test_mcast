#include "time_handler.h"

int64_t TimeHandler::g_curTime  = 0;
bool TimeHandler::g_isReady     = false;

int TimeHandler::StartTick(void* arg, pthread_attr_t *attr)
{
    UpdateTime((void*)1);
    pthread_t       tid;
    int     rc      = pthread_create(&tid, attr, &TimeHandler::UpdateTime, arg);
    if (rc != 0)
    {
        return -1;
    }
    if (attr == NULL)
    {
        pthread_detach(tid);
    }
    for (int iLoop = 0; iLoop < 10; iLoop ++)
    {
        if (g_isReady)
        {
            return 0;
        }
        usleep(100 * 1000);
    }
    return -1;
}

void* TimeHandler::UpdateTime(void* arg)
{
    do
    {
        int64_t oldTime = g_curTime;
        timeval tvCur;
        gettimeofday(&tvCur, NULL);
        int64_t tCurTime    = 0;
        tCurTime    = tvCur.tv_sec * 1000 * 1000 + tvCur.tv_usec;
        __sync_bool_compare_and_swap(&g_curTime, oldTime, tCurTime);
        usleep(50);
    } while (!arg);
    g_isReady       = true;
}

