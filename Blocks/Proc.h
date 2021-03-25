//
// Created by zhujiaying on 2021/3/16.
//

#ifndef SIMPLATFORM_PROC_H
#define SIMPLATFORM_PROC_H

#include "../BasicBlock.h"

class Proc : public BasicBlock
{
public:
    Proc(int ninput, int sizeofinitem, int noutput, int sizeofoutitem);
    ~Proc() override;

private:
    virtual int work(int noutput, std::vector<const void *> &input, std::vector<void *> &output) override;

    virtual void forecast(int noutput, int &ninput_required) override;
};

#endif //SIMPLATFORM_PROC_H
