//
// Created by zhujiaying on 2021/3/24.
//

#include "MsgGenerator.h"
#include <time.h>
#include <iostream>

static long long int num = 0;

MsgGenerator::MsgGenerator(int nbytes)
    : BasicBlock(0,0,1,sizeof(uint8_t)),
      _nbytes(nbytes)
{
    unsigned seed = time(0);
    srand(seed);
}

MsgGenerator::~MsgGenerator() noexcept
{

}

void MsgGenerator::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

int MsgGenerator::work(int noutput, std::vector<const void *> &input, std::vector<void *> &output)
{
    std::cout << "------------------------" << std::endl;
    std::cout << "nbytes=" << _nbytes << std::endl;
    std::cout << "noutput=" << noutput << std::endl;
    if (_nbytes == 0)
        return -1;

    uint8_t* out = (uint8_t*)output[0];


    int i = 0;
    for (; i<noutput && _nbytes>0; ++i)
    {
        out[i] = (uint8_t)(rand() % 255);
        --_nbytes;
        std::cout << (int)out[i] << " ";
    }
    std::cout << std::endl;

    return i;
}