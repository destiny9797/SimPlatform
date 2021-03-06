//
// Created by zhujiaying on 2021/3/29.
//

#include "Interface.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <semaphore.h>


#define NITEMS ( 8 * (1L<<10) )

Interface::Interface(std::string fname, int sizeofitem, CommMode mode)
    : _fname(fname),
      _sizeof_interface(sizeofitem),
      _count(0),
      _mode(mode)
{
    char* pathname = getcwd(NULL,0);
    if (pathname == NULL){
        std::runtime_error("Interface[Interface]: Wrong path.");
    }
    _fname = std::string(pathname) + "/../tmp/vm-" + fname;
    int fd = open(_fname.c_str(), O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        perror("open shared file failed :");
        throw std::runtime_error("Interface[Interface]: open shared file failed.");
    }

    int filesize = NITEMS*sizeofitem;
    if (ftruncate(fd, (off_t)(2*filesize)) == -1)
    {
        close(fd);
        throw std::runtime_error("Interface[Interface]: ftruncate 1 failed.");
    }

    void* firstcopy = mmap(NULL, 2*filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)0);
    if (firstcopy == MAP_FAILED)
    {
        close(fd);
        throw std::runtime_error("Interface[Interface]: mmap 1 failed.");
    }

    if (munmap((char*)firstcopy+filesize, filesize) == -1)
    {
        close(fd);
        throw std::runtime_error("Interface[Interface]: munmap failed.");
    }

    void* secondcopy = mmap((char*)firstcopy+filesize, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)0);
    if (secondcopy == MAP_FAILED)
    {
        munmap(firstcopy, filesize);
        close(fd);
        throw std::runtime_error("Interface[Interface]: mmap 2 failed.");
    }

    if (secondcopy != (char*)firstcopy+filesize)
    {
        munmap(firstcopy, filesize);
        munmap(secondcopy, filesize);
        close(fd);
        throw std::runtime_error("Interface[Interface]: two address not continuous.");
    }

    if (ftruncate(fd, (off_t)filesize) == -1)
    {
        munmap(firstcopy, filesize);
        munmap(secondcopy, filesize);
        close(fd);
        throw std::runtime_error("Interface[Interface]: ftruncate 2 failed.");
    }

    _base = (char*)firstcopy;
    close(fd);

    std::string rwpfilename = std::string(pathname) + "/../tmp/vm-rw_index_" + fname;
    fd = open(rwpfilename.c_str(), O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        perror("open shared file failed :");
        throw std::runtime_error("Interface[Interface]: open rw_index file failed.");
    }
    if (ftruncate(fd, (off_t)(4*sizeof(int))) == -1)
    {
        close(fd);
        throw std::runtime_error("Interface[Interface]: ftruncate rw_index to 8 bytes failed.");
    }
    void* rwpointer = mmap(NULL, 4*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)0);
    _writeind = (int*)rwpointer;
    _readind = (int*)rwpointer+1;
    _done1 = (int*)rwpointer+2;
    _opened = (int*)rwpointer+3;

    if (*_opened != 1)
    {
        *_writeind = 0;
        *_readind = 0;
        *_done1 = 0;
    }
    *_opened = 1;

    _bufsize = NITEMS;
    close(fd);

}
Interface::~Interface() noexcept
{
    *_done1 = 1;
    *_opened = 0;
    munmap(_base, 2*NITEMS*_sizeof_interface);
    munmap((char*)_writeind, 2*sizeof(int));
    std::cout << "count=" << _count << std::endl;
//    std::cout << "~Interface()" << std::endl;
}

SinkInterface::SinkInterface(spTranState state, std::string fname, int sizeofitem, CommMode mode)
    : BasicBlock("SinkApi",1,sizeofitem,1,sizeofitem),
      Interface(fname, sizeofitem, mode),
      _transtate(state)
{
    SetType(SINKAPI);
//    std::string sem1_name = fname + "_1";
//    std::string sem2_name = fname + "_2";
//    sem_t* sem1 = sem_open(sem1_name.c_str(), O_CREAT, 0666, 0);
//    sem_t* sem2 = sem_open(sem2_name.c_str(), O_CREAT, 0666, 0);
//    if (sem1 == SEM_FAILED)
//    {
//        perror("sem1 open error");
//        throw std::runtime_error("Interface[Interface]: sem_open 1 failed.");
//    }
//    if (sem2 == SEM_FAILED)
//    {
//        perror("sem2 open error");
//        throw std::runtime_error("Interface[Interface]: sem_open 2 failed.");
//    }
//    sem_post(sem1);
//    sem_wait(sem2);
//
//    sem_close(sem1);
//    sem_close(sem2);
//    sem_unlink(sem1_name.c_str());
//    sem_unlink(sem2_name.c_str());
}

SinkInterface::~SinkInterface() noexcept {}

void SinkInterface::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

int SinkInterface::work(int noutput, int& ninput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const char* in = (const char*)input[0];
    char* out = (char*)output[0];

    int nchar = _sizeof_interface / sizeof(char);

    for (int i=0; i<noutput; ++i)
    {
        for (int j=0; j<nchar; ++j)
        {
            *out = *in;
            out++;
            in++;
        }
    }

    _count += noutput;
    ninput = noutput;
    return noutput;
}

SourceInterface::SourceInterface(spTranState state, std::string fname, int sizeofitem, CommMode mode)
    : BasicBlock("SourceApi",1,sizeofitem,1,sizeofitem),
      Interface(fname, sizeofitem, mode),
      _transtate(state)
{
    SetType(SOURCEAPI);
//    std::string sem1_name = fname + "_1";
//    std::string sem2_name = fname + "_2";
//    sem_t* sem1 = sem_open(sem1_name.c_str(), O_CREAT, 0666, 0);
//    sem_t* sem2 = sem_open(sem2_name.c_str(), O_CREAT, 0666, 0);
//    if (sem1 == SEM_FAILED)
//    {
//        perror("sem1 open error");
//        throw std::runtime_error("Interface[Interface]: sem_open 1 failed.");
//    }
//    if (sem2 == SEM_FAILED)
//    {
//        perror("sem2 open error");
//        throw std::runtime_error("Interface[Interface]: sem_open 2 failed.");
//    }
//    sem_post(sem2);
//    sem_wait(sem1);
//
//    sem_close(sem1);
//    sem_close(sem2);
//    sem_unlink(sem1_name.c_str());
//    sem_unlink(sem2_name.c_str());
}

SourceInterface::~SourceInterface() noexcept {}

void SourceInterface::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

int SourceInterface::work(int noutput, int& ninput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const char* in = (const char*)input[0];
    char* out = (char*)output[0];

    int nchar = _sizeof_interface / sizeof(char);

    for (int i=0; i<noutput; ++i)
    {
        for (int j=0; j<nchar; ++j)
        {
            *out = *in;
            out++;
            in++;
        }
    }

    _count += noutput;
    ninput = noutput;
    return noutput;
}
