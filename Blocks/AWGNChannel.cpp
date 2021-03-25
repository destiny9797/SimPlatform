//
// Created by zhujiaying on 2021/3/24.
//

#include "AWGNChannel.h"
#include <complex>
#include <random>
#include <math.h>

AWGNChannel::AWGNChannel(float noise_power)
    : BasicBlock(1,sizeof(std::complex<float>),1,sizeof(std::complex<float>)),
      _En(noise_power),
      _generator(),
      _dist_real(0.0,std::sqrt(noise_power/2)),
      _dist_imag(0.0,std::sqrt(noise_power/2))
{

}

AWGNChannel::~AWGNChannel() noexcept
{

}

void AWGNChannel::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

int AWGNChannel::work(int noutput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const float* in = (const float*)input[0];
    float* out = (float*)output[0];

    for (int i=0; i<noutput; ++i)
    {
        out[0] = in[0] + _dist_real(_generator);
        out[1] = in[1] + _dist_imag(_generator);

        in += 2;
        out += 2;
    }

    return noutput;
}