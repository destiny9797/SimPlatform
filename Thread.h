//
// Created by zhujiaying on 2021/3/13.
//

#ifndef SIMPLATFORM_THREAD_H
#define SIMPLATFORM_THREAD_H


#include <functional>
#include <vector>
#include <thread>
#include <condition_variable>

class BasicBlock;
class Buffer;
class BufferReader;

class Thread
{
public:
    typedef std::unique_ptr<std::thread> pthread;
    typedef std::weak_ptr<BasicBlock> wpBasicBlock;
    typedef std::shared_ptr<BasicBlock> spBasicBlock;
    typedef std::shared_ptr<Buffer> spBuffer;
    typedef std::shared_ptr<BufferReader> spBufferReader;

    typedef std::function<int(int, std::vector<const void*>&, std::vector<void*>&)> Callback_BlockRun;
//    typedef std::function<void(spBasicBlock, bool)> Callback_Notify;

    enum State {
        READY,           // We made progress; everything's cool.
        BLKD_IN,         // no progress; we're blocked waiting for input data.
        BLKD_OUT,        // no progress; we're blocked waiting for output buffer space.
        DONE,            // we're done; don't call me again.
    };

    Thread(wpBasicBlock block);
    ~Thread();

    void SetLinkblock(wpBasicBlock block);

    void SetBlockrunCallback(const Callback_BlockRun& cb){ _blockrun = cb; }

//    void SetNotifyCallback(const Callback_Notify& cb){ _notify = cb; }

    void CreateThread();

    void SetInchanged()
    {
        std::unique_lock<std::mutex> lk(_mutex);
        _inchanged = true;
        _cond.notify_one(); //其实是通知自己
    }

    void SetOutchanged()
    {
        std::unique_lock<std::mutex> lk(_mutex);
        _outchanged = true;
        _cond.notify_one();
    }

    void ClearChanged(){ _inchanged = false; _outchanged = false; }

    void Wait();

    bool _done;

private:
    void NotifyUpblocks(spBasicBlock blk);

    void NotifyDownblocks(spBasicBlock blk);

    bool UpBlocksDone(spBasicBlock blk);

    void Scheduler();

    State Work();

    wpBasicBlock _block;

    pthread _thr;

    int _ninput_required;

    int _decitmation;

    int _interpolation;

    int _ninport;

    int _noutport;

    std::vector<const void*> _input;

    std::vector<void*> _output;

    std::condition_variable _cond;

    std::mutex _mutex;

    bool _outchanged;

    bool _inchanged;

    Callback_BlockRun _blockrun;

//    Callback_Notify _notify;
};

#endif //SIMPLATFORM_THREAD_H
