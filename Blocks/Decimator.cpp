//
// Created by zhujiaying on 2021/4/26.
//

#include "Decimator.h"

Decimator::Decimator(std::string name, int decimation, std::vector<float> &taps)
    : BasicBlock(name,1,sizeof(float),1,sizeof(float)),
      _decimation(decimation),
      _taps(taps)
{
    SetHistory(_taps.size());
    SetDecimation(_decimation);
}

Decimator::~Decimator() noexcept
{}

void Decimator::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput*GetDecimation() + GetHistory() - 1;
}

int Decimator::work(int noutput, int &ninput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const float* in = (const float*)input[0];
    float* out = (float*)output[0];

    for (int i=0; i<noutput*_decimation; ++i)
    {
        float asum = 0.0;
        for (int j=i; j<i+GetHistory(); ++j)
        {
            asum += in[j]*_taps[j-i];
        }
        if (i%_decimation==0)
        {
            out[i/_decimation] = asum;
        }
    }

    ninput = noutput*_decimation;
    return noutput;
}