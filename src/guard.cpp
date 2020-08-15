#include "guard.h"
#include "mutex.h"

CThreadGuard::CThreadGuard(CMutex *mutex): m_mutex(NULL) {
    if (mutex) {
        m_mutex = mutex;
        m_mutex->lock();
    }
}

CThreadGuard::~CThreadGuard() {
    if (m_mutex) {
        m_mutex->unlock();
    }
}