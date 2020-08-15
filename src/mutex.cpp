#include "mutex.h"

CMutex::CMutex()
{
    const int ret = pthread_mutex_init(&m_mutex, NULL);
}

CMutex::~CMutex()
{
    pthread_mutex_destroy(&m_mutex);
}

void CMutex::lock()
{
    pthread_mutex_lock(&m_mutex);
}
void CMutex::trylock()
{
    pthread_mutex_trylock(&m_mutex);
}
void CMutex::unlock()
{
    pthread_mutex_unlock(&m_mutex);
}