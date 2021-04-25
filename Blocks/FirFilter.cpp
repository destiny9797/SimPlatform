//
// Created by zhujiaying on 2021/4/25.
//

#include "FirFilter.h"

FirFilter::FirFilter(std::string name, std::vector<float> &taps)
    : BasicBlock(name,1,sizeof(float),1,sizeof(float)),
      _ntaps(taps.size()),
      _taps(taps)
{
    SetHistory(_ntaps);
}

FirFilter::~FirFilter() noexcept
{}

void FirFilter::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput + GetHistory() - 1;
}

int FirFilter::work(int noutput, int &ninput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const float* in = (const float*)input[0];
    float* out = (float*)output[0];


    for (int i=0; i<noutput; ++i)
    {
        float asum = 0.0;
        for (int j=i; j<i+_ntaps; ++j)
        {
            asum += in[j]*_taps[j-i];
        }
        out[i] = asum;
    }

    ninput = noutput;
    return noutput;
}