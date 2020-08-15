#ifndef UTILS_RUNNABLE_H_
#define UTILS_RUNNABLE_H_

class CThread;
/*CRunnable是一个抽象类，它拥有一个run纯虚方法，主要用于CThread类*/
class CRunnable {
public:
    virtual ~CRunnable(){}
    virtual void run(CThread *thread, void *argv) = 0;
};

#endif 