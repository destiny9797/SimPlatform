//
// Created by zhujiaying on 2021/3/13.
//

#ifndef SIMPLATFORM_THREADPOOL_H
#define SIMPLATFORM_THREADPOOL_H

#include "Thread.h"
#include <functional>
#include <vector>
#include <unordered_map>

class BasicBlock;
class ThreadPool
{
public:
    typedef std::function<void()> Functor;
    typedef std::shared_ptr<Thread> spThread;
    typedef std::shared_ptr<BasicBlock> spBasicBlock;
    ThreadPool();
    ~ThreadPool();

    void Start();

    void Wait();

    void Stop();

    void AddThread(const spThread& thr);


private:
    std::vector<spThread> _threadlist;
};

#endif //SIMPLATFORM_THREADPOOL_H
