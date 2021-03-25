//
// Created by zhujiaying on 2021/3/24.
//

#include "SRRCFilter.h"
#include <complex>

SRRCFilter::SRRCFilter(float rolloff, int ntaps, int interpolation)
    : BasicBlock(1,sizeof(std::complex<float>),1,sizeof(std::complex<float>)),
      _rolloff(rolloff),
      _ntaps(ntaps)
{
    SetHistory(_ntaps);
    SetInterpolation(interpolation);
}

SRRCFilter::~SRRCFilter() noexcept
{

}

void SRRCFilter::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput / GetInterpolation() + GetHistory() - 1;
}

int SRRCFilter::work(int noutput, std::vector<const void *> &input, std::vector<void *> &output)
{

}