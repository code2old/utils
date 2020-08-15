#ifndef UTILS_THREAD_POOL_H_
#define UTILS_THREAD_POOL_H_

#include "runnable.h"
#include "cond.h"
#include "thread.h"

#ifdef USE_STD_STL
#include <queue>
#include <vector>
using namespace std;
#else
#endif

typedef void (*func)(void *owner, void *data);

typedef struct TaskItem {
    int priority;
    func callback;
    void *owner;
    void *data;
    int operator<(const TaskItem &rhs) const {
        return (priority < rhs.priority);
    }
} TASK_ITEM;

typedef enum WorkStatus {
    WORK_STATUS_STOP    = 0,
    WORK_STATUS_START   = 1,
    WORK_STATUS_RUNNING = 2,
    WORK_STATUS_WAIT    = 3,
    WORK_STATUS_EXIT    = 4,
} WORK_STATUS;

typedef struct WorkerItem {
    int                             index;
    CThread                         thread;
    CCond                           cond;
    WorkStatus                      status;
    priority_queue<TaskItem *>      taskQ;
    bool                            waitFinished;
} WORKER_ITEM;

/*简单线程池*/
class CThreadPool: public CRunnable {
public:
    CThreadPool(int threadCount = 1);
    virtual ~CThreadPool();
    void setThreadCount(int threadCount);
    void start();
    void stop(bool waitFinish = false);
    void startWorker(func callback, void *owner, void *data, int priority);
    virtual void run(CThread* thread, void *arg);
protected:
    int m_threadCount;              /*线程数量*/
    unsigned int m_currIndex;       /*当前通道*/
    vector<WorkerItem *>  m_threadQ;  /*工作线程执行队列*/
    CCond m_freeTaskCond;          /*信号*/
    queue<TaskItem *>  m_freeTaskQ; /*空闲任务队列*/

    unsigned int m_totalTaskNum;
    unsigned int m_freeTaskNum;
    unsigned int m_workingTaskNum;
};

#endif 