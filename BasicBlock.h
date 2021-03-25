//
// Created by zhujiaying on 2021/3/12.
//

#ifndef SIMPLATFORM_BASICBLOCK_H
#define SIMPLATFORM_BASICBLOCK_H

#include "Buffer.h"
#include "Thread.h"
#include <unistd.h>
#include <memory>


class BasicBlock : public std::enable_shared_from_this<BasicBlock>
{
public:
    typedef std::shared_ptr<Buffer> spBuffer;
    typedef std::shared_ptr<BufferReader> spBufferReader;
    typedef std::shared_ptr<Thread> spThread;
    virtual ~BasicBlock();

    int GetInputPortNum(){ return _ninput; }
    int GetOutputPortNum(){ return _noutput; }

    int GetInputSizeofitem(){ return _sizeofinitem; }
    int GetOutputSizeofitem(){ return _sizeofoutitem; }

    void SetNOutbuf(int n){ _outbuflist.resize(n); }
    void SetNInbuf(int n){ _inbuflist.resize(n); }
    void SetOutbuffer(int port, spBuffer buffer);
    void SetInbuffer(int port, spBufferReader bufferreader);

    void SetHistory(int n){ _history = n; }
    int GetHistory(){ return _history; }

    void SetDecimation(int n){ _decimation = n; }
    int GetDecimation(){ return _decimation; }

    void SetInterpolation(int n){ _interpolation = n; }
    int GetInterpolation(){ return _interpolation; }

    spBuffer GetOutbuffer(int port){ return _outbuflist[port]; }
    spBufferReader GetInbuffer(int port){ return _inbuflist[port]; }

    spThread GetThread(){ return _thread; }

    //为thread注册回调函数
    void ThreadRegister();

    inline void SetInchanged()
    {
        _thread->SetInchanged();
    }

    inline void SetOutchanged()
    {
        _thread->SetOutchanged();
    }

    inline bool Done()
    {
        return _thread->_done;
    }

    virtual void forecast(int noutput, int& ninput_required) = 0;

protected:
    BasicBlock(int ninput, int sizeofinitem, int noutput, int sizeofoutitem);

    virtual int work(int noutput, std::vector<const void*>& input, std::vector<void*>& output) = 0;

private:
    int _ninput;
    int _noutput;
    int _sizeofinitem;
    int _sizeofoutitem;

    int _history;

    int _decimation;

    int _interpolation;

    std::vector<spBuffer> _outbuflist;
    std::vector<spBufferReader> _inbuflist;


    spThread _thread;

};

#endif //SIMPLATFORM_BASICBLOCK_H
