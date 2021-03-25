//
// Created by zhujiaying on 2021/3/24.
//

#include "BPSKDemod.h"
#include <complex>

BPSKDemod::BPSKDemod()
    : BasicBlock(1,sizeof(std::complex<float>),1,sizeof(char))
{

}

BPSKDemod::~BPSKDemod() noexcept
{

}

void BPSKDemod::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

int BPSKDemod::work(int noutput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const std::complex<float>* in = (const std::complex<float>*)input[0];
    char* out = (char*)output[0];

    for (int i=0; i<noutput; ++i)
    {
        out[i] = in[i].real()>0 ? 1 : 0;
    }

    return noutput;
}