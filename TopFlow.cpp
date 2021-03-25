//
// Created by zhujiaying on 2021/3/12.
//

#include "TopFlow.h"



TopFlow::TopFlow()
    : _connections(),
      _threadpool()
{

}

TopFlow::~TopFlow()
{
//    std::cout << "~TopFLow()" << std::endl;
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
void TopFlow::Run()
{

    _threadpool.Start();
}


void TopFlow::Start()
{
    _connections.CheckConn();

    _connections.SetupConn();

    MakeThread();

//    Run();
}

void TopFlow::Wait()
{
    _threadpool.Wait();
}
