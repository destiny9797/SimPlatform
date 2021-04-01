//
// Created by zhujiaying on 2021/3/15.
//

#ifndef SIMPLATFORM_NULLSINK_H
#define SIMPLATFORM_NULLSINK_H

#include "../BasicBlock.h"

class NullSink : public BasicBlock
{
public:
    NullSink(std::string name, int ninput, int sizeofinitem, int noutput, int sizeofoutitem);
    ~NullSink() override;

private:
    virtual int work(int noutput, std::vector<const void*>& input, std::vector<void*>& output) override;

    virtual void forecast(int noutput, int& ninput_required) override;

};

#endif //SIMPLATFORM_NULLSINK_H
