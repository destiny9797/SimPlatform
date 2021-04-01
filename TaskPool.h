//
// Created by zhujiaying on 2021/3/31.
//

#ifndef SIMPLATFORM_TASKPOOL_H
#define SIMPLATFORM_TASKPOOL_H

#include <queue>
#include <functional>

class TaskPool
{
public:
    typedef std::function<int(int, uint8_t*)> Task;

    TaskPool();

    ~TaskPool();

    Task GetTask();

    void AddTask(Task task);

    void PopTask();

private:
    std::queue<Task> _taskqueue;

};

#endif //SIMPLATFORM_TASKPOOL_H
