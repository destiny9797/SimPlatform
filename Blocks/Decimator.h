//
// Created by zhujiaying on 2021/4/26.
//

#ifndef SIMPLATFORM_DECIMATOR_H
#define SIMPLATFORM_DECIMATOR_H

#include "../BasicBlock.h"

class Decimator : public BasicBlock
{
public:
    Decimator(std::string name, int decimation, std::vector<float>& taps);

    ~Decimator() override;

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output) override;

    int _decimation;

    std::vector<float> _taps;
};

#endif //SIMPLATFORM_DECIMATOR_H
