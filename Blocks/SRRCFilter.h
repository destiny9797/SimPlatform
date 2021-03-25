//
// Created by zhujiaying on 2021/3/24.
//

#ifndef SIMPLATFORM_SRRC_H
#define SIMPLATFORM_SRRC_H

#include "../BasicBlock.h"

class SRRCFilter : public BasicBlock
{
public:
    SRRCFilter(float rolloff, int ntaps, int interpolation);

    ~SRRCFilter() override;

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, std::vector<const void*>& input, std::vector<void*>& output) override;

    float _rolloff;

    int _ntaps;
};

#endif //SIMPLATFORM_SRRC_H
