#include <stdio.h>
#include <unistd.h>

#include "threadPool.h"

CThreadPool::CThreadPool(int threadCount) {
    m_threadCount = threadCount;
    m_currIndex = 0;
    m_totalTaskNum = 0;
    m_freeTaskNum = 0;
    m_workingTaskNum = 0;
}

CThreadPool::~CThreadPool() {
    /*等待所有线程退出*/
    stop();
    bool taskFinished = false;
    TaskItem *task = NULL;
    WorkerItem *worker = NULL;
    vector<WorkerItem *>::iterator it;
    
    while (m_threadQ.size()) {
        for (it = m_threadQ.begin(); it != m_threadQ.end(); /**/) {
            taskFinished = false;
            worker = (*it);
            worker->cond.lock();
            if (WORK_STATUS_EXIT == worker->status && worker->taskQ.empty())
            {
                taskFinished = true;
            }
            worker->cond.unlock();
            if (taskFinished)
            {
                it = m_threadQ.erase(it);
                delete worker;
                worker = NULL;
            } else {
                ++it;
            }
        }
        sleep(10);
    }
    while (!m_freeTaskQ.empty())
    {
        task = m_freeTaskQ.front();
        m_freeTaskQ.pop();
        delete task;
        task = NULL;
    }
}

void CThreadPool::start()
{
    WorkerItem *worker = NULL;
    for (int i=0; i<m_threadCount; ++i) 
    {
        worker = new WorkerItem;
        worker->index = i;
        worker->status = WORK_STATUS_STOP;
        worker->thread.start(this, (void *)worker);
        m_threadQ.push_back(worker);
    }
}

void CThreadPool::stop(bool waitFinish)
{
    WorkerItem *worker = NULL;
    for (int i=0; i<m_threadQ.size(); ++i)
    {
        worker = m_threadQ[i];
        worker->cond.lock();
        worker->status = WORK_STATUS_EXIT;
        worker->waitFinished = waitFinish;
        worker->cond.unlock();
        worker->cond.signal();
    }
}

void CThreadPool::startWorker(func callback, void *owner, void *data, int priority)
{
    TaskItem *task = NULL;
    /*将任务hash到各个线程中，注意：多线程调用startWorker时可能出现两个任务hash到同一个线程中(m_currIndex++非原子操作)，但是影响不大，暂不处理*/
    WorkerItem *worker = m_threadQ[(m_currIndex++) % m_threadQ.size()];

    m_freeTaskCond.lock();
    int freeTaskQLen = m_freeTaskQ.size();
    if (freeTaskQLen == 0)
    {
        task = new TaskItem;
        m_totalTaskNum++;
    }
    else
    {
        task = m_freeTaskQ.front();
        m_freeTaskQ.pop();
        m_freeTaskNum--;
    }
    m_freeTaskCond.unlock();

    if (NULL == task)
    {
        /*TODO: task为空表明出现错误，此时对于任务总数的统计并不准确*/
        return; 
    }

    m_workingTaskNum++;
    task->callback = callback;
    task->owner = owner;
    task->data = data;
    task->priority = priority;

    worker->cond.lock();
    if (worker->status != WORK_STATUS_EXIT)
    {
        worker->taskQ.push(task);
        worker->cond.signal();
    }
    worker->cond.unlock();
}

void CThreadPool::run(CThread* thread, void *arg)
{
    /*thread实际上是worker->thread的CThread指针，arg为worker指针*/
    if (NULL == thread)
    {
        printf("CThreadPool::run(CThread* thread{NULL}, void *arg) invalid param\n");
        return;
    }
    if (NULL == arg)
    {
        printf("CThreadPool::run(CThread* thread, void *arg{NULL}) invalid param\n");
        return;
    }
    TaskItem *task = NULL;                                  /*需要执行的任务*/
    WorkerItem *worker = (WorkerItem *)arg;                 /*工作线程*/
    worker->status = WORK_STATUS_START;
    while (worker->status != WORK_STATUS_EXIT)
    {
        worker->cond.lock();
        while (worker->status != WORK_STATUS_EXIT && worker->taskQ.size() == 0) {
            worker->status = WORK_STATUS_WAIT;
            worker->cond.wait();
        }
        if (WORK_STATUS_EXIT == worker->status)
        {
            worker->cond.unlock();
            break;
        }
        worker->status = WORK_STATUS_RUNNING;

        task = worker->taskQ.top();
        worker->taskQ.pop();
        worker->cond.unlock();

        if (NULL == task)
        {
            continue;
        }
        if (task->callback != NULL)
        {
            task->callback(task->owner, task->data);
        }

        /*程序运行正确的话，m_freeTaskNum与m_freeTaskQ.size()应该是保持一致的*/
        m_freeTaskCond.lock();
        m_freeTaskNum++;
        m_freeTaskQ.push(task);
        m_freeTaskCond.unlock();
    }
    /*是否等待queue中的task做完*/
    while (worker->taskQ.size() > 0)
    {
        task = worker->taskQ.top();
        worker->taskQ.pop();
        if (NULL == task)
        {
            continue;
        }
        if (worker->waitFinished && task->callback != NULL)
        {
            task->callback(task->owner, task->data);
        }

        /*程序运行正确的话，m_freeTaskNum与m_freeTaskQ.size()应该是保持一致的*/
        m_freeTaskCond.lock();
        m_freeTaskNum++;
        m_freeTaskQ.push(task);
        m_freeTaskCond.unlock();
    }    
}
