//
// Created by zhujiaying on 2021/3/13.
//

#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool()
    : _threadlist()
{

}

ThreadPool::~ThreadPool()
{
//    std::cout << "~ThreadPool()" << std::endl;
}

void ThreadPool::AddThread(const spThread &thr)
{
    _threadlist.emplace_back(thr);
}



void ThreadPool::Start()
{
    for (spThread& thr : _threadlist)
    {
        thr->CreateThread();
    }
}

void ThreadPool::Wait()
{
    for (spThread& thr : _threadlist)
    {
        thr->Wait();
    }
}

void ThreadPool::Stop()
{
    for (spThread& thr : _threadlist)
    {
        thr->Stop();
    }
}