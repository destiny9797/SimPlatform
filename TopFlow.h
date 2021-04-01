//
// Created by zhujiaying on 2021/3/12.
//

#ifndef SIMPLATFORM_TOPFLOW_H
#define SIMPLATFORM_TOPFLOW_H

#include "BasicBlock.h"
#include "Connection.h"
#include "ThreadPool.h"
#include "TaskPool.h"

class TopFlow
{
public:
    typedef std::function<int(int, uint8_t*)> Task;
    typedef std::shared_ptr<BasicBlock> spBasicBlock;
    typedef Connection::Pair Pair;
    TopFlow();
    ~TopFlow();

    //开启线程池
    void Run();

    void Start();

    void Wait();

    void Stop();

    //连接block的收发关系
    void Connect(spBasicBlock blk1, int port1, spBasicBlock blk2, int port2);

    void AddTask(Task task);

    static std::mutex _apimutex;

    static std::condition_variable _apicond;

    static bool _hassinkapi;

    static bool _hassourceapi;


private:

    void SetTask();

    //为每个block设置线程，加入线程池
    void MakeThread();

    Connection _connections;

    TaskPool _taskpool;

    ThreadPool _threadpool;


};


#endif //SIMPLATFORM_TOPFLOW_H
