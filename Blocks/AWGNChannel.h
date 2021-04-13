//
// Created by zhujiaying on 2021/3/24.
//

#ifndef SIMPLATFORM_AWGNCHANNEL_H
#define SIMPLATFORM_AWGNCHANNEL_H

#include "../BasicBlock.h"
#include <random>
#include <complex>

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

#endif //SIMPLATFORM_AWGNCHANNEL_H
