//
// Created by zhujiaying on 2021/3/13.
//

#include "Thread.h"
#include "BasicBlock.h"
#include "Interface.h"
#include "Buffer.h"
#include <iostream>
#include <time.h>


static std::mutex _smutex;

Thread::Thread(wpBasicBlock block)
    : _block(block),
      _ninput_required(0),
      _inchanged(false),
      _outchanged(false),
      _done(false),
      _thr(nullptr)
{
    if (!_block.expired())
    {
        spBasicBlock blk = _block.lock();
        _ninport = blk->GetInputPortNum();
        _noutport = blk->GetOutputPortNum();

        _input.resize(_ninport);
        _output.resize(_noutport);

        _decitmation = blk->GetDecimation();
        _interpolation = blk->GetInterpolation();
    }

}


Thread::~Thread()
{
    //bug: 如果模块只定义了没有连接，则为nullptr
    // 把Thread的创建放到连接的时候
//    std::cout << "~Thread" << std::endl;
    if (_thr!=nullptr && _thr->joinable())
        _thr->join();
}

void Thread::Wait()
{
    assert(_thr != nullptr);
    if (_thr->joinable())
        _thr->join();
}

void Thread::CreateThread()
{
    _thr = std::make_unique<std::thread>(std::bind(&Thread::Scheduler, this));
}


void Thread::Scheduler()
{
    State state;
    spBasicBlock blk = _block.lock();
    if (blk == nullptr)
        throw std::runtime_error("Thread[Scheduler]: the block has been destructed.");
    if (blk->isSinkInterface())
    {
        std::shared_ptr<SinkInterface> spSinkblk = std::dynamic_pointer_cast<SinkInterface,BasicBlock>(blk);
        if (spSinkblk == nullptr)
            throw std::runtime_error("Thread[Scheduler]: dynamic pointer cast to Sink failed.");

        while (1)
        {
            ClearChanged();
            state = SinkWork(spSinkblk);
            switch (state)
            {
                case READY:
                    NotifyUpblocks(spSinkblk);
                    //不通知收进程
                    break;
                case BLKD_IN:
                {
                    std::unique_lock<std::mutex> lk(_mutex);
                    _cond.wait(lk, [this](){return _inchanged;});
                }
                    break;
                case BLKD_OUT:
                    //不断检查输出buffer有没有空位
                    break;
                case DONE:
                    NotifyUpblocks(spSinkblk);
                    //通知收进程已完成
                    spSinkblk->SetDone();
                    return;

                default:
                    throw std::runtime_error("Thread[Scheduler]: Wrong state of SinkInterface.");
            }
        }


    }
    else if (blk->isSourceInterface())
    {
        std::shared_ptr<SourceInterface> spSourceblk = std::dynamic_pointer_cast<SourceInterface,BasicBlock>(blk);
        if (spSourceblk == nullptr)
            throw std::runtime_error("Thread[Scheduler]: dynamic pointer cast to Source failed.");

        while (1)
        {
            ClearChanged();
            state = SourceWork(spSourceblk);
            switch (state)
            {
                case READY:
                    //不通知发进程
                    NotifyDownblocks(spSourceblk);
                    break;
                case BLKD_IN:
                    //不断检查是否有新数据了
                    break;
                case BLKD_OUT:
                {
                    std::unique_lock<std::mutex> lk(_mutex);
                    _cond.wait(lk, [this](){return _outchanged;});
                }
                    break;
                case DONE:
                    //将共享内存的_done标志设为1
                    spSourceblk->SetDone();
                    NotifyDownblocks(spSourceblk);
                    return;

                default:
                    throw std::runtime_error("Thread[Scheduler]: Wrong state of SourceInterface.");
            }
        }
    }
    else
    {
        while (1)
        {
            //每个线程只会清零自己的条件变量，但是会设置邻居的条件变量为true，每个线程只需监控自己的条件变量
            ClearChanged();
            state = Work();
            switch (state)
            {
                case READY:
                    NotifyUpblocks(blk);
                    NotifyDownblocks(blk);
                    break;
                case BLKD_IN:
                {
//                    time_t start, end;
//                    start = clock();
                    std::unique_lock<std::mutex> lk(_mutex);
                    _cond.wait(lk, [this](){return _inchanged;});
//                    end = clock();
//                    std::cout << "blkd_in wait: " << (double)(end-start)/CLOCKS_PER_SEC*1000 << "ms" << std::endl;
                }
                    break;
                case BLKD_OUT:
                {
//                    time_t start, end;
//                    start = clock();
                    std::unique_lock<std::mutex> lk(_mutex);
                    _cond.wait(lk, [this](){return _outchanged;});
//                    end = clock();
//                    std::cout << "blkd_out wait: " << (double)(end-start)/CLOCKS_PER_SEC*1000 << "ms" << std::endl;
                }
                    break;
                case DONE:
                    NotifyUpblocks(blk);
                    NotifyDownblocks(blk);
                    return;

                default:
                    throw std::runtime_error("Thread[Scheduler]: Wrong state.");
            }
        }
    }

}

Thread::State Thread::SinkWork(spSink blk)
{
    int noutput = blk->CalcAvailSpace();
    if (noutput == 0)
    {
        if (blk->isDone() == 1)
        {
            _done = true;
            return DONE;
        }
        return BLKD_OUT;
    }
    int avail_data = blk->GetInbuffer(0)->CalcAvailData();
    if (avail_data <= 0)
    {
        if (UpBlocksDone(blk))
        {
            _done = true;
            return DONE;
        }
        return BLKD_IN;
    }
    noutput = std::min(noutput,avail_data);
    _output[0] = blk->GetWritepointer();
    _input[0] = blk->GetInbuffer(0)->GetReadpointer();

    int nproduce = _blockrun(noutput, _input, _output);
    blk->Produce(nproduce);
    blk->GetInbuffer(0)->Consume(noutput);

    return READY;
}

Thread::State Thread::SourceWork(spSource blk)
{
    int noutput = blk->GetOutbuffer(0)->CalcAvailSpace();
    if (noutput == 0)
    {
        if (DownBlocksDone(blk))
        {
            _done = true;
            return DONE;
        }
        return BLKD_OUT;
    }

    int avail_data = blk->CalcAvailData();
    if (avail_data <= 0)
    {
        if (blk->isDone() == 1)
        {
            _done = true;
            return DONE;
        }
        return BLKD_IN;
    }

    noutput = std::min(noutput, avail_data);
    _output[0] = blk->GetOutbuffer(0)->GetWritepointer();
    _input[0] = blk->GetReadpointer();

    int nproduce = _blockrun(noutput, _input, _output);
    blk->GetOutbuffer(0)->Produce(nproduce);
    blk->Consume(noutput);

    return READY;
}

Thread::State Thread::Work()
{
    //block分为三类，一类是source只有输出，一类是sink只有输入，还有一类是proc既有输出也有输入
    //一个source模块可以输出的数据只取决于输出buffer有多少位置空闲
    //一个sink模块可以输入的数据只取决于上一级产生了多少
    //一个proc模块可以处理的数据数量取决于两个，一个是输入多少数据，一个是输出可用空间


    spBasicBlock blk = _block.lock();
    if (blk == nullptr)
        throw std::runtime_error("Thread[Work]: block has been destructed");

    //可用输出空间大小，以一个数据类型为单位，如int类型就是可产生多少个int数据
    int min_avail_space = INT_MAX;
    for (int port=0; port<_noutport; ++port)
    {
        spBuffer buffer = blk->GetOutbuffer(port);
        int avail_space = buffer->CalcAvailSpace() / _interpolation * _interpolation; //使之为插值率的倍数
        min_avail_space = std::min(min_avail_space, avail_space);

        _output[port] = buffer->GetWritepointer();
    }
//    {
//        std::unique_lock<std::mutex> lk(_smutex);
//        std::cout << "Thread id: " << std::this_thread::get_id() << ", min_avail_space: " << min_avail_space << std::endl;
//    }


    if (min_avail_space == 0)
    {
        if (DownBlocksDone(blk))
        {
            _done = true;
            return DONE;
        }
        return BLKD_OUT;
    }

    //仅用最大空间的一半，提高并行性 -------- 测试性能还不如有多少用多少
//    if (_block->GetInputPortNum()==0 && _block->GetOutputPortNum()>0 && _block->GetOutbuffer(0)->GetBufreader().size()>1)
//    {
////        min_avail_space /= 2;
//        min_avail_space = std::min(min_avail_space,_block->GetOutbuffer(0)->GetBufsize() / 2);
//        min_avail_space /= _interpolation * _interpolation;
//    }

    //上一级block已经产生了多少数据，以一个数据类型为单位，如int类型就是可读取多少个int数据
    int min_avail_data = INT_MAX;
    for (int port=0; port<_ninport; ++port)
    {
        spBufferReader reader = blk->GetInbuffer(port);
        int avail_data = reader->CalcAvailData();
        min_avail_data = std::min(min_avail_data, avail_data);

        _input[port] = reader->GetReadpointer();
    }

//    {
//        std::unique_lock<std::mutex> lk(_smutex);
//        std::cout << "Thread id: " << std::this_thread::get_id() << ", min_avail_data: " << min_avail_data << std::endl;
//    }

    //算出现有输入能产生多少输出，如果少于可用输出空间，则压
    int noutput = min_avail_space;
    int noutput_required = (min_avail_data - blk->GetHistory() + 1) / _decitmation * _interpolation;
    noutput_required = noutput_required / _interpolation * _interpolation;
    if (noutput_required <= 0)
    {
        //如果上一级blocks都已经done了，并且输入也都读完了，则本block也done了
        if (UpBlocksDone(blk))
        {
            _done = true;
            return DONE;
        }
        return BLKD_IN;
    }
    if (noutput_required < min_avail_space)
    {
        noutput = noutput_required;
    }

//    {
//        std::unique_lock<std::mutex> lk(_smutex);
//        std::cout << "Thread id: " << std::this_thread::get_id() << ", noutput[before]: " << noutput << std::endl;
//    }

    while (noutput > 0)
    {
        //用户定义的forecast函数，用于获知需要多少输入
        blk->forecast(noutput, _ninput_required);
        if (_ninput_required > min_avail_data)
        {
            noutput /= 2;
            noutput_required = noutput_required / _interpolation * _interpolation;
        }
        else
            break;
    }

//    {
//        std::unique_lock<std::mutex> lk(_smutex);
//        std::cout << "Thread id: " << std::this_thread::get_id() << ", noutput[after]: " << noutput << std::endl;
//    }
    if (noutput == 0)
        return BLKD_OUT;

    //假设block的消耗和生产数量相等，如果有插值、采样，要乘除
    int ninput = noutput / _interpolation * _decitmation;

    //为啥需要返回产生了多少，不就是noutput吗？
    //因为
    int nproduce = _blockrun(noutput, _input, _output);
    if (nproduce == -1)
    {
        _done = true;
        return DONE;
    }
    for (int port=0; port<_noutport; ++port)
    {
        blk->GetOutbuffer(port)->Produce(nproduce);
    }
    for (int port=0; port<_ninport; ++port)
    {
        blk->GetInbuffer(port)->Consume(ninput);
    }
    return READY;
}

void Thread::NotifyUpblocks(spBasicBlock blk)
{
    for (int i=0; i<_ninport; ++i)
    {
        blk->GetInbuffer(i)->GetBuffer().lock()->GetBlock().lock()->SetOutchanged();
//        _notify(_block->GetInbuffer(i)->GetBuffer()->GetBlock(), false);
//        _block->GetInbuffer(i)->GetBuffer()->GetBlock()->GetThread()->SetOutchanged();
    }


}

void Thread::NotifyDownblocks(spBasicBlock blk)
{
    for (int i=0; i<_noutport; ++i)
    {
        std::vector<spBufferReader> readers = blk->GetOutbuffer(i)->GetBufreader();
        for (spBufferReader& reader : readers)
        {
            //bug, 不能从block获取线程，会导致Thread析构，应该从线程池里找要通知的block对应的线程
            //true表示SetInchanged，false表示SetOutchanged
            reader->GetBlock().lock()->SetInchanged();
//             _notify(reader->GetBlock(), true);
        }
    }
}

bool Thread::UpBlocksDone(spBasicBlock blk)
{
    //输入数据不够的情况下，只要有一个upBlock结束了，就认为我也结束了
    for (int i=0; i<_ninport; ++i)
    {
        if (blk->GetInbuffer(i)->GetBuffer().lock()->GetBlock().lock()->Done())
            return true;
    }
    return false;
}

bool Thread::DownBlocksDone(spBasicBlock blk)
{
    //输出空间不够，只要有一个downBlock结束了，就认为我也结束了
    for (int i=0; i<_noutport; ++i)
    {
        std::vector<spBufferReader> readers = blk->GetOutbuffer(i)->GetBufreader();
        for (spBufferReader reader : readers)
        {
            if (!(reader->GetBlock().lock()->Done()))
                return true;
        }
    }
    return false;
}