//
// Created by zhujiaying on 2021/3/24.
//

#ifndef SIMPLATFORM_BITGENERATOR_H
#define SIMPLATFORM_BITGENERATOR_H

#include "../BasicBlock.h"

class MsgGenerator : public BasicBlock
{
public:
    MsgGenerator(std::string name, int nbytes);

    ~MsgGenerator() override;

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, std::vector<const void*>& input, std::vector<void*>& output) override;

    int _nbytes;
};

#endif //SIMPLATFORM_BITGENERATOR_H
