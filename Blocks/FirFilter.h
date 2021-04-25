//
// Created by zhujiaying on 2021/4/25.
//

#ifndef SIMPLATFORM_FIRFILTER_H
#define SIMPLATFORM_FIRFILTER_H

#include "../BasicBlock.h"

class FirFilter : public BasicBlock
{
public:
    FirFilter(std::string name, std::vector<float>& taps);

    ~FirFilter() override;

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output) override;

    std::vector<float> _taps;

    int _ntaps;

    int _interpolation;
};

#endif //SIMPLATFORM_FIRFILTER_H
