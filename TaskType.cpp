//
// Created by zhujiaying on 2021/4/1.
//

#include "TaskType.h"

std::queue<spcache> caches;

int randombytes(int noutput, unsigned char* out)
{
    for (int i=0; i<noutput; ++i)
    {
        out[i] = rand()%255;
    }
    return noutput;
}
int echo(int noutput, unsigned char* out)
{
    if (caches.empty())
        return 0;
    spcache cache = caches.front();
    int w = cache->_writeind;
    int r = cache->_readind;
    int remain = w-r;
    if (remain <= noutput)
    {
        for (int i=0; i<remain; ++i)
        {
            out[i] = cache->cache[r++];
        }
        caches.pop();
        return remain;
    }
    else
    {
        for (int i=0; i<noutput; ++i)
        {
            out[i] = cache->cache[r++];
        }
        cache->_readind = r;
        return noutput;
    }
}

int ack(int noutput, unsigned char* out)
{
    if (noutput < 3)
        return -3;
    out[0] = 'a';
    out[1] = 'c';
    out[2] = 'k';
    return 3;
}

int gettime(int noutput, unsigned char* out)
{

}