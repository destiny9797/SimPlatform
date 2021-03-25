//
// Created by zhujiaying on 2021/3/24.
//

#ifndef SIMPLATFORM_BPSKDEMOD_H
#define SIMPLATFORM_BPSKDEMOD_H

#include "../BasicBlock.h"

class BPSKDemod : public BasicBlock
{
public:
    BPSKDemod();

    ~BPSKDemod() override;

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, std::vector<const void*>& input, std::vector<void*>& output) override;
};

#endif //SIMPLATFORM_BPSKDEMOD_H
