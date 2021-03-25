//
// Created by zhujiaying on 2021/3/24.
//

#ifndef SIMPLATFORM_BPSK_H
#define SIMPLATFORM_BPSK_H

#include "../BasicBlock.h"

class BPSK : public BasicBlock
{
public:
    BPSK();

    ~BPSK() override;

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, std::vector<const void*>& input, std::vector<void*>& output) override;
};

#endif //SIMPLATFORM_BPSK_H
