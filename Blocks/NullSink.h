//
// Created by zhujiaying on 2021/3/15.
//

#ifndef SIMPLATFORM_NULLSINK_H
#define SIMPLATFORM_NULLSINK_H

#include "../BasicBlock.h"

template <class T>
class NullSink : public BasicBlock
{
public:
    NullSink(std::string name);
    ~NullSink() override;

private:
    virtual int work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output) override;

    virtual void forecast(int noutput, int& ninput_required) override;

};

#endif //SIMPLATFORM_NULLSINK_H
