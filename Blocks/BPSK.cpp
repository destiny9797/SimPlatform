//
// Created by zhujiaying on 2021/3/24.
//

#include "BPSK.h"
#include <complex>
#include <iostream>

BPSK::BPSK(std::string name)
    : BasicBlock(name, 1,sizeof(char),1,sizeof(std::complex<float>))
{

}

BPSK::~BPSK() noexcept
{
}

void BPSK::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

int BPSK::work(int noutput, int& ninput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const char* in = (const char*)input[0];
    float* out = (float*)output[0];

    for (int i=0; i<noutput; ++i)
    {
        out[0] = in[i]==1 ? 1 : -1;
        out[1] = 0;
        out += 2;
//        std::cout << (int)in[i] << " ";
    }

    ninput = noutput;

    return noutput;
}
