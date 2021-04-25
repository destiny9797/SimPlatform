//
// Created by zhujiaying on 2021/4/21.
//

#ifndef SIMPLATFORM_MULTIPLY_H
#define SIMPLATFORM_MULTIPLY_H

#include "../BasicBlock.h"

template <class T>
class Multiply : public BasicBlock
{
public:
    Multiply(std::string name);

    ~Multiply() override;

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output) override;

};

#endif //SIMPLATFORM_MULTIPLY_H
