//
// Created by zhujiaying on 2021/3/12.
//

#include "Buffer.h"
#include "BasicBlock.h"
#include <iostream>
#include <sys/mman.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>

int Buffer::s_granularity = 4096;

Buffer::Buffer(wpBasicBlock block, int nitems, int sizeofitem)
    : _block(block),
      _sizeofitem(sizeofitem),
      _bufsize(0),
      _writeind(0),
      _basebuffer(nullptr),
      _readerlist()
{
    if (_block.lock()->GetType() != BasicBlock::SINKAPI)
        AllocateBuffer(nitems,sizeofitem);
}

Buffer::~Buffer()
{
//    std::cout << "~Buffer()" << std::endl;
    munmap(_basebuffer, 2*_bufsize*_sizeofitem);
}

void Buffer::AllocateBuffer(int nitems, int sizeofitem)
{
    //分配buffer，并实现循环buffer
    //循环buffer，有两个指针指向两段连续地址，但是两个指针都映射到同一个文件描述符，也就是说两个指针都可以用来更改文件内容
    int granularity = Buffer::s_granularity;
    int nitem_per_page = granularity / sizeofitem;
    if (nitems % nitem_per_page != 0)
    {
        nitems = ((nitems/nitem_per_page)+1) * nitem_per_page;
    }
    _bufsize = nitems;

    int size = nitems * sizeofitem;

    static int s_buffer_counter = 0;
    std::string buffer_name = "/Users/zhujiaying/github/SimPlatform/tmp/vm-" +
                                std::to_string(getpid()) + "-" + std::to_string(s_buffer_counter);
    s_buffer_counter++;

    //打开一个文件，返回文件描述符
    int fd = open(buffer_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0777);
    if (fd == -1)
    {
        throw std::runtime_error("Buffer[AllocateBuffer]: open buffer file failed.");
    }

    //将文件expand到 2*size 大小
    if (ftruncate(fd,(off_t)(2*size)) == -1)
    {
        close(fd);
        throw std::runtime_error("Buffer[AllocateBuffer]: ftruncate 1 failed.");
    }

    //mmap映射，指针指向文件数据，映射 2*size 的数据
    void* firstcopy = mmap(NULL, 2*size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)0);
    if (firstcopy == MAP_FAILED)
    {
        close(fd);
        throw std::runtime_error("Buffer[AllocateBuffer]: mmap 1 failed.");
    }
    //将后面一半unmap
    if (munmap((char*)firstcopy+size, size) == -1)
    {
        close(fd);
        perror("munmap:");
        throw std::runtime_error("Buffer[AllocateBuffer]: munmap failed.");
    }

    //mmap映射，second指针依旧指向文件前size部分，但是是从first+size处开始的
    void* secondcopy = mmap((char*)firstcopy+size, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)0);
    if (secondcopy == MAP_FAILED)
    {
        munmap(firstcopy, size);
        close(fd);
        throw std::runtime_error("Buffer[AllocateBuffer]: mmap 2 failed.");
    }

    //检查连续性
    if (secondcopy != (char*)firstcopy + size)
    {
        munmap(firstcopy, size);
        munmap(secondcopy, size);
        close(fd);
        throw std::runtime_error("Buffer[AllocateBuffer]: two address not continuous.");
    }
    //将文件截断，只剩前size部分
    if (ftruncate(fd,(off_t)size) == -1)
    {
        munmap(firstcopy, size);
        munmap(secondcopy, size);
        close(fd);
        throw std::runtime_error("Buffer[AllocateBuffer]: ftruncate 2 failed.");
    }
    close(fd);

    _basebuffer = (char*)firstcopy;
}

int Buffer::CalcAvailSpace()
{
    int max_avail_data = 0;
    for (spBufferReader& reader : _readerlist)
    {
        max_avail_data = std::max(max_avail_data, reader->CalcAvailData());
//        std::cout << "reader index is " << reader->_readind << ", write index is " << _writeind << ", minspace is " << _bufsize-max_avail_data-1 << std::endl;
    }

    return _bufsize - max_avail_data - 1;
}

BufferReader::BufferReader(wpBasicBlock block, wpBuffer buffer)
    : _block(block),
      _buffer(buffer)
{
    spBasicBlock blk = block.lock();
    if (blk == nullptr)
        throw std::runtime_error("BufferReader[BufferReader]: the block has been destructed.");
    spBuffer buf = buffer.lock();
    if (buf == nullptr)
        throw std::runtime_error("BufferReader[BufferReader]: the buffer has been destructed.");
    _readind = buf->MinusInd(buf->_writeind, blk->GetHistory()-1);
}

BufferReader::~BufferReader()
{
//    std::cout << "~BufferReader()" << std::endl;
}

int BufferReader::CalcAvailData()
{
    return _buffer.lock()->MinusInd(_buffer.lock()->_writeind, _readind);
}
