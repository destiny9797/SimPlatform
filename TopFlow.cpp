//
// Created by zhujiaying on 2021/3/12.
//

#include "TopFlow.h"
#include "MsgControl.h"

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>


std::mutex TopFlow::_apimutex;
std::condition_variable TopFlow::_apicond;
bool TopFlow::_hassinkapi = false;
bool TopFlow::_hassourceapi = false;

TopFlow::TopFlow()
    : _connections(),
      _threadpool()
{

}

TopFlow::~TopFlow()
{
    ClearTmpfile("/Users/zhujiaying/github/SimPlatform/tmp/");
//    std::cout << "~TopFLow()" << std::endl;
}

void TopFlow::ClearTmpfile(const char* path)
{

    DIR *dir;
    struct dirent *dirinfo;
    struct stat statbuf;
    char filepath[256] = {0};
    lstat(path, &statbuf);


    if ((dir = opendir(path)) == NULL)
        return;
    while ((dirinfo = readdir(dir)) != NULL)
    {
        const char* filename = dirinfo->d_name;
        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)//判断是否是特殊目录
            continue;
        if (strlen(filename)>=3 && filename[0]=='v' && filename[1]=='m' && filename[2]=='-')
        {
            strcpy(filepath,path);
            if(filepath[strlen(path) - 1] != '/')
                strcat(filepath, "/");
            strcat(filepath, dirinfo->d_name);
            printf("path is = %s\n",filepath);
            remove(filepath);
        }
    }
    closedir(dir);
}


void TopFlow::Connect(spBasicBlock blk1, int port1, spBasicBlock blk2, int port2)
{
    if (blk1 == nullptr || blk2 == nullptr)
    {
        throw std::runtime_error("TopFlow[Connect]: block is null.");
    }

    //检查端口号是否超出范围
    if (port1 < 0 || port1 >= blk1->GetOutputPortNum()
        || port2 < 0 || port2 >= blk2->GetInputPortNum())
    {
        throw std::runtime_error("TopFlow[Connect]: block port number is wrong.");
    }

    //检查数据类型是否匹配
    if (blk1->GetOutputSizeofitem() != blk2->GetInputSizeofitem())
    {
        throw std::runtime_error("TopFlow[Connect]: data type not match.");
    }

    //检查输入端口是否已使用，因为每个输入端口只能有一个输入流，但是每个输出端口可以输出到多个口
    if (_connections.CheckUsed(blk2,port2))
    {
        throw std::runtime_error("TopFlow[Connect]: the input port has been used.");
    }

    //注册这对连接
    Pair apair(BlkPort(blk1,port1),BlkPort(blk2,port2));
    _connections.Register(apair);
}

void TopFlow::MakeThread()
{
    //将每个block的work函数传入其thread，即为thread设置回调函数，并将该thread加入线程池
    std::vector<spBasicBlock> blocks = _connections.GetUsedBlocks();
    for (spBasicBlock block : blocks)
    {
        block->ThreadRegister();
        _threadpool.AddThread(block->GetThread());
    }
}

void TopFlow::SetTask()
{
    std::vector<spBasicBlock> blocks = _connections.GetUsedBlocks();
    for (spBasicBlock block : blocks)
    {
        if (block->GetType() == BasicBlock::MSGGEN)
        {
            std::shared_ptr<MsgGenerater> blk = std::dynamic_pointer_cast<MsgGenerater,BasicBlock>(block);
            if (blk == nullptr)
                throw std::runtime_error("TopFlow[SetupConn]: cast to MsgGenerater failed.");
            blk->SetTaskpool(std::make_shared<TaskPool>(_taskpool));
        }
        else if (block->GetType() == BasicBlock::MSGPARSER)
        {
            std::shared_ptr<MsgParser> blk = std::dynamic_pointer_cast<MsgParser,BasicBlock>(block);
            if (blk == nullptr)
                throw std::runtime_error("TopFlow[SetupConn]: cast to MsgParser failed.");
            blk->SetTaskpool(std::make_shared<TaskPool>(_taskpool));
        }
    }
}

void TopFlow::AddTask(Task task)
{
    _taskpool.AddTask(task);
}

void TopFlow::Run()
{

    _threadpool.Start();
}


void TopFlow::Start()
{
    _connections.CheckConn();

    _connections.SetupConn();

    SetTask();

    MakeThread();


//    Run();
}

void TopFlow::Wait()
{
    _threadpool.Wait();
}

void TopFlow::Stop()
{
    _threadpool.Stop();
}
