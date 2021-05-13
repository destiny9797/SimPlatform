//
// Created by zhujiaying on 2021/3/24.
//

#ifndef SIMPLATFORM_AWGNCHANNEL_H
#define SIMPLATFORM_AWGNCHANNEL_H

#include "../BasicBlock.h"
#include <random>
#include <complex>
#include <math.h>

template <class T>
class AWGNChannel : public BasicBlock
{
public:
    AWGNChannel(std::string name, float noise_power);

    ~AWGNChannel() override;

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output) override;

    float _En;

    std::default_random_engine _generator;

    std::normal_distribution<float> _dist_real;
    std::normal_distribution<float> _dist_imag;
};


/*
 * 普通类型数据，一般都默认为float类型
 */


template <>
AWGNChannel<float>::AWGNChannel(std::string name, float noise_power)
        : BasicBlock(name, 1,sizeof(float),1,sizeof(float)),
          _En(noise_power),
          _generator(),
          _dist_real(0.0,std::sqrt(noise_power))
{

}

template <class T>
AWGNChannel<T>::~AWGNChannel()
{

}

template <class T>
void AWGNChannel<T>::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

template <>
int AWGNChannel<float>::work(int noutput, int& ninput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const float* in = (const float*)input[0];
    float* out = (float*)output[0];

    for (int i=0; i<noutput; ++i)
    {
        out[i] = in[i] + _dist_real(_generator);
    }

    ninput = noutput;
    return noutput;
}



/*
 * 复数类型的数据
 */



template <>
AWGNChannel<std::complex<float>>::AWGNChannel(std::string name, float noise_power)
        : BasicBlock(name, 1,sizeof(std::complex<float>),1,sizeof(std::complex<float>)),
          _En(noise_power),
          _generator(),
          _dist_real(0.0,std::sqrt(noise_power/2)),
          _dist_imag(0.0,std::sqrt(noise_power/2))
{

}

template <>
int AWGNChannel<std::complex<float>>::work(int noutput, int& ninput, std::vector<const void *> &input, std::vector<void *> &output)
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

    ninput = noutput;
    return noutput;
}

#endif //SIMPLATFORM_AWGNCHANNEL_H
