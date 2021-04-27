//
// Created by zhujiaying on 2021/4/26.
//

#ifndef SIMPLATFORM_BITSYNCGARDNER_H
#define SIMPLATFORM_BITSYNCGARDNER_H

#include "../BasicBlock.h"

class BitsyncGardner : public BasicBlock
{
public:
    BitsyncGardner(std::string name);

    ~BitsyncGardner() override;

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output) override;

};

#endif //SIMPLATFORM_BITSYNCGARDNER_H
