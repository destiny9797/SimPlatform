//
// Created by zhujiaying on 2021/3/31.
//

#include "TaskPool.h"

TaskPool::TaskPool()
{

}

TaskPool::~TaskPool()
{

}

TaskPool::Task TaskPool::GetTask()
{
    if (_taskqueue.empty())
        return nullptr;
    return _taskqueue.front();
}

void TaskPool::AddTask(Task task)
{
    _taskqueue.push(task);

}

void TaskPool::PopTask()
{
    _taskqueue.pop();
}