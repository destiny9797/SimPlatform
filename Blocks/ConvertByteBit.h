//
// Created by zhujiaying on 2021/3/24.
//

#ifndef SIMPLATFORM_CONVERTBYTEBIT_H
#define SIMPLATFORM_CONVERTBYTEBIT_H

#include "../BasicBlock.h"


class ConvertByteBit : public BasicBlock
{
public:
    ConvertByteBit(std::string name, bool byte_to_bit = true);

    ~ConvertByteBit() override;

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output) override;

    bool _bytetobit;

};

#endif //SIMPLATFORM_CONVERTBYTEBIT_H
