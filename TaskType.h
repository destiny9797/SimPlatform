//
// Created by zhujiaying on 2021/3/31.
//

#ifndef SIMPLATFORM_TASKTYPE_H
#define SIMPLATFORM_TASKTYPE_H

#include <vector>
#include <queue>

#define MAXCACHE 4096

class Cache
{
public:
    Cache(): _writeind(0), _readind(0), cache(MAXCACHE){}

    int _writeind;

    int _readind;

    std::vector<unsigned char> cache;

};

typedef std::shared_ptr<Cache> spcache;

int randombytes(int noutput, unsigned char* out);

int echo(int noutput, unsigned char* out);

int ack(int noutput, unsigned char* out);

int gettime(int noutput, unsigned char* out);

#endif //SIMPLATFORM_TASKTYPE_H
