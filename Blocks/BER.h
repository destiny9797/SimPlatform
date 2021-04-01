//
// Created by zhujiaying on 2021/3/24.
//

#ifndef SIMPLATFORM_BER_H
#define SIMPLATFORM_BER_H

#include "../BasicBlock.h"

class BER : public BasicBlock
{
public:
    BER(std::string name);

    ~BER() override;

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, std::vector<const void*>& input, std::vector<void*>& output) override;

    int _ntotalbit;

    int _nerrorbit;
};

#endif //SIMPLATFORM_BER_H
