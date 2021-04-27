//
// Created by zhujiaying on 2021/3/24.
//

#include "BPSKDemod.h"
#include <complex>
#include <iostream>

BPSKDemod::BPSKDemod(std::string name)
    : BasicBlock(name, 1,sizeof(float),1,sizeof(char))
{

}

BPSKDemod::~BPSKDemod() noexcept
{

}

void BPSKDemod::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

int BPSKDemod::work(int noutput, int& ninput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const float* in = (const float*)input[0];
    char* out = (char*)output[0];

//    std::unique_lock<std::mutex> lk(BasicBlock::_blockmutex);

//    std::cout << "thread: " << std::this_thread::get_id() << std::endl;
//    std::cout << "noutput= " << noutput << std::endl;

    for (int i=0; i<noutput; ++i)
    {
        out[i] = in[i]>0 ? 1 : 0;
//        std::cout << (int)out[i] << " ";
    }

//    std::cout << std::endl;
    ninput = noutput;
    return noutput;
}