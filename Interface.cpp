//
// Created by zhujiaying on 2021/3/29.
//

#include "Interface.h"
#include <sys/mman.h>
#include <fcntl.h>

#define NITEMS ( 8 * (1L<<10) )

Interface::Interface(std::string fname, int sizeofitem)
    : BasicBlock(1, sizeofitem, 1, sizeofitem),
      _fname(fname),
      _sizeof_interface(sizeofitem)
{
    int fd = open(_fname.c_str(), O_RDWR | O_CREAT);
    if (fd == -1)
    {
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

    close(fd);

    std::string rwpfilename = "rw_index";
    fd = open(rwpfilename.c_str(), O_RDWR | O_CREAT);
    if (fd == -1)
    {
        throw std::runtime_error("Interface[Interface]: open rw_index file failed.");
    }
    if (ftruncate(fd, (off_t)(3*sizeof(int))) == -1)
    {
        close(fd);
        throw std::runtime_error("Interface[Interface]: ftruncate rw_index to 8 bytes failed.");
    }
    void* rwpointer = mmap(NULL, 3*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)0);
    _writeind = (int*)rwpointer;
    _readind = (int*)rwpointer+1;
    _done1 = (int*)rwpointer+2;

    *_done1 = 0;

    _bufsize = filesize;
    close(fd);

}
Interface::~Interface() noexcept
{
    *_done1 = 1;
    munmap(_base, 2*NITEMS*_sizeof_interface);
    munmap((char*)_writeind, 2*sizeof(int));
}

SinkInterface::SinkInterface(std::string fname, int sizeofitem, noise_model noisemodel, fade_model fademodel)
    : Interface(fname, sizeofitem)
{
    SetSink();
}

SinkInterface::~SinkInterface() noexcept {}

void SinkInterface::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

int SinkInterface::work(int noutput, std::vector<const void *> &input, std::vector<void *> &output)
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
    return noutput;
}

SourceInterface::SourceInterface(std::string fname, int sizeofitem)
    : Interface(fname, sizeofitem)
{
    SetSource();
}

SourceInterface::~SourceInterface() noexcept {}

void SourceInterface::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

int SourceInterface::work(int noutput, std::vector<const void *> &input, std::vector<void *> &output)
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
    return noutput;
}