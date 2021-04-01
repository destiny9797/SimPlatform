//
// Created by zhujiaying on 2021/3/31.
//

#include "MsgControl.h"
#include "TaskType.h"
#include <iostream>

MsgGenerater::MsgGenerater(spTranState state)
    : BasicBlock("MsgGen",0,0,1,sizeof(uint8_t)),
      _transtate(state)
{
    SetType(MSGGEN);
}

MsgGenerater::~MsgGenerater() noexcept
{

}

void MsgGenerater::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

int MsgGenerater::work(int noutput, std::vector<const void *> &input, std::vector<void *> &output)
{
    Task task = _taskpool->GetTask();
    if (task == nullptr)
        return -2;

    uint8_t* out = (uint8_t*)output[0];
    int nproduce = task(noutput,out);

    if (nproduce == 0) //任务已完成
        _taskpool->PopTask();
    else if (nproduce == -3) //noutput不够，等下一轮
        return 0;

    return nproduce;
}

MsgParser::MsgParser(spTranState state)
    : BasicBlock("MsgParser",1,sizeof(uint8_t),0,0),
      _transtate(state)
{
    SetType(MSGPARSER);
}

MsgParser::~MsgParser() noexcept
{

}

void MsgParser::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

int MsgParser::work(int noutput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const uint8_t* in = (const uint8_t*)input[0];
//    if (noutput > MAXCACHE)
//        std::cout << "echo length is longer than 4096." << std::endl;
//    else
//    {
//        spcache cache = std::make_shared<Cache>();
//        caches.push(cache);
//        int w = cache->_writeind;
//        for (int i=0; i<noutput; ++i)
//        {
//            cache->cache[w++] = in[i];
//        }
//        cache->_writeind = w;
//    }
    for (int i=0; i<noutput; ++i)
    {
        std::cout << (int)in[i] << " ";
    }
    return noutput;
}