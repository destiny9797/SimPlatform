//
// Created by zhujiaying on 2021/3/12.
//

#ifndef SIMPLATFORM_BUFFER_H
#define SIMPLATFORM_BUFFER_H

#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>

class BasicBlock;
class BufferReader;
class Buffer
{
public:
    friend class BufferReader;
    typedef std::weak_ptr<BasicBlock> wpBasicBlock;
    typedef std::shared_ptr<BasicBlock> spBasicBlock;
    typedef std::shared_ptr<BufferReader> spBufferReader;
    static int s_granularity;

    Buffer(wpBasicBlock block, int nitems, int sizeofitem);
    ~Buffer();

    std::vector<spBufferReader> GetBufreader(){ return _readerlist; }

    wpBasicBlock GetBlock(){ return _block; }

    int GetBufsize(){ return _bufsize; }

    void* GetWritepointer()
    {
        return &_basebuffer[_writeind * _sizeofitem];
    }

    //返回以字节为单位的可用空间
    int CalcAvailSpace();

    void Produce(int n)
    {
        _writeind = AddInd(_writeind, n);
    }

    void AllocateBuffer(int nitems, int sizeofitem);

    void AddReader(spBufferReader br){ _readerlist.emplace_back(br); }



private:
    int AddInd(int x, int y)
    {
        int res = x + y;
        if (res >= _bufsize)
            res -= _bufsize;
        assert(res>=0 && res<_bufsize);
        return res;
    }

    int MinusInd(int x, int y)
    {
        int res = x - y;
        if (res < 0)
            res += _bufsize;
        assert(res>=0 && res<_bufsize);
        return res;
    }

    wpBasicBlock _block;

    //buffer大小，以字节为单位
    int _bufsize;

    int _sizeofitem;

    int _writeind;

    char* _basebuffer;


    std::vector<spBufferReader> _readerlist;
};




class BufferReader
{
public:
    friend class Buffer;
    typedef std::weak_ptr<Buffer> wpBuffer;
    typedef std::shared_ptr<Buffer> spBuffer;
    typedef Buffer::wpBasicBlock wpBasicBlock;
    typedef Buffer::spBasicBlock spBasicBlock;
    BufferReader(wpBasicBlock block, wpBuffer buffer);
    ~BufferReader();

    void* GetReadpointer()
    {
        return &(_buffer.lock()->_basebuffer)[_readind * _buffer.lock()->_sizeofitem];
    }

    wpBuffer GetBuffer(){ return _buffer; }

    wpBasicBlock GetBlock(){return _block; }

    int CalcAvailData();

    void Consume(int n)
    {
        _readind = _buffer.lock()->AddInd(_readind, n);
    }

private:
    wpBuffer _buffer;

    wpBasicBlock _block;

    int _readind;
};

#endif //SIMPLATFORM_BUFFER_H
