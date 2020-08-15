#ifndef UTILS_COND_H_
#define UTILS_COND_H_

#include <pthread.h>
#include "mutex.h"

class CCond: public CMutex
{
public:
    CCond();
    ~CCond();
    bool wait(int milliseconds = 0);
    void signal();
    void broadcast();

private:
    pthread_cond_t m_cond;
};

#endif