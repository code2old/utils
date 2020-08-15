#ifndef UTILS_THREAD_H_
#define UTILS_THREAD_H_

#include <pthread.h>
#include "runnable.h"

class CThread {
public:
    CThread();
    /*启动线程*/
    void start(CRunnable *r, void *data);
    /*pthread_join*/
    void join();
    /*获取可执行类对象*/
    CRunnable *getRunnable();
    /*获取参数*/
    void *getArgs();
    int getpid();
    /*线程执行*/
    static void *hook(void *args);

private:  
    pthread_t m_tid;
    int m_pid;
    CRunnable *m_runnable;
    void *m_args;
};

#endif 