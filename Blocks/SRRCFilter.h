//
// Created by zhujiaying on 2021/3/24.
//

#ifndef SIMPLATFORM_SRRC_H
#define SIMPLATFORM_SRRC_H

#include "../BasicBlock.h"


class SRRCFilter : public BasicBlock
{
public:
    SRRCFilter(std::string name, float gain, float rolloff, int ntaps, int interpolation);

    ~SRRCFilter() override;

private:
    const double PI = 3.14159265358979323846;

    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output) override;

    void makeTaps();

    float _gain;

    float _rolloff;

    int _ntaps;

    int _sps;

    std::vector<float> _taps;
};

#endif //SIMPLATFORM_SRRC_H
