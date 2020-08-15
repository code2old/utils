#include <sys/time.h>
#include <sys/types.h>

#include "cond.h"

CCond::CCond()
{
    pthread_cond_init(&m_cond, NULL);
}
CCond::~CCond()
{
    pthread_cond_destroy(&m_cond);
}
bool CCond::wait(int milliseconds)
{
    bool ret = true;
    if (0 == milliseconds)
    {
        pthread_cond_wait(&m_cond, &m_mutex);
    }
    else
    {
        struct timeval curtime;
        struct timespec abstime;
        gettimeofday(&curtime, NULL);
        /*微秒*/
        int64_t us = (static_cast<int64_t>(curtime.tv_sec) * 
                        static_cast<int64_t>(1000000) + 
                        static_cast<int64_t>(curtime.tv_usec) + 
                        static_cast<int64_t>(milliseconds) * 
                        static_cast<int64_t>(1000));
        abstime.tv_sec  = static_cast<int>(us / static_cast<int64_t>(1000000));
        abstime.tv_nsec = static_cast<int>(us % static_cast<int64_t>(1000000) * 1000);
        ret = (pthread_cond_timedwait(&m_cond, &m_mutex, &abstime) == 0);
    }
    return ret;
}

void CCond::signal()
{
    pthread_cond_signal(&m_cond);
}
void CCond::broadcast()
{
    pthread_cond_broadcast(&m_cond);
}