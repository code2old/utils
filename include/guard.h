#ifndef UTILS_GUARD_H_
#define UTILS_GUARD_H_

class CMutex;
class CThreadGuard {
public:
    CThreadGuard(CMutex *mutex);
    ~CThreadGuard();
private:
    CMutex *m_mutex;
};

#endif 