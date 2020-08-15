#ifndef UTILS_MUTEX_H_
#define UTILS_MUTEX_H_

#include <pthread.h>

class CMutex {
public:
    CMutex();
    virtual ~CMutex();
    void lock();
    void trylock();
    void unlock();
protected:
    pthread_mutex_t m_mutex;
};

#endif 