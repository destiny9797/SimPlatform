//
// Created by zhujiaying on 2021/3/12.
//

#include "BasicBlock.h"
#include <iostream>

BasicBlock::BasicBlock(std::string name, int ninput, int sizeofinitem, int noutput, int sizeofoutitem)
    : _name(name),
      _ninput(ninput),
      _noutput(noutput),
      _sizeofinitem(sizeofinitem),
      _sizeofoutitem(sizeofoutitem),
      _history(1),
      _decimation(1),
      _interpolation(1),
      _outbuflist(noutput),
      _inbuflist(ninput),
      _type(NORMAL),
      _thread(nullptr)
{

}

BasicBlock::~BasicBlock()
{
//    std::cout << "~BasicBlock()" << std::endl;
}

void BasicBlock::SetOutbuffer(int port, spBuffer buffer)
{
    if (port<0 || port>=_noutput)
        throw std::runtime_error("BasicBlock[SetOutBuffer]: wrong port.");
    _outbuflist[port] = buffer;
}

void BasicBlock::SetInbuffer(int port, spBufferReader bufferreader)
{
    if (port<0 || port>=_ninput)
        throw std::runtime_error("BasicBlock[SetInBuffer]: wrong port.");
    _inbuflist[port] = bufferreader;
}

void BasicBlock::ThreadRegister()
{
    _thread = std::make_shared<Thread>(shared_from_this());
//    _thread->SetLinkblock(shared_from_this());
    _thread->SetBlockrunCallback(std::bind(&BasicBlock::work,
                                           this,
                                           std::placeholders::_1,
                                           std::placeholders::_2,
                                           std::placeholders::_3,
                                           std::placeholders::_4));
}
