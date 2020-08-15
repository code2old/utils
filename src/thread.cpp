#include <unistd.h>
#include <sys/syscall.h>

#include "thread.h"

#define gettid() syscall(SYS_gettid)

CThread::CThread() {
    m_tid = 0;
    m_runnable = NULL;
    m_args = NULL;
}

void CThread::start(CRunnable *r, void *data) {
    m_runnable = r;
    m_args = data;
    pthread_create(&m_tid, NULL, CThread::hook, this);
}

void CThread::join() {
    if (m_tid) {
        pthread_join(m_tid, NULL);
        m_tid = 0;
    }
}

CRunnable *CThread::getRunnable() {
    return m_runnable;
}

void *CThread::getArgs() {
    return m_args;
}

void *CThread::hook(void *args) {
    CThread *thread = (CThread *)args;
    thread->m_tid = gettid();
    if (thread->getRunnable()) {
        thread->getRunnable()->run(thread, thread->getArgs());
    }
    return NULL;
}