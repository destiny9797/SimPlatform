//
// Created by zhujiaying on 2021/3/13.
//

#ifndef SIMPLATFORM_NULLSOURCE_H
#define SIMPLATFORM_NULLSOURCE_H

#include "../BasicBlock.h"

class NullSource : public BasicBlock
{
public:
    NullSource(std::string name, int ninput, int sizeofinitem, int noutput, int sizeofoutitem);
    ~NullSource() override;

private:
    virtual int work(int noutput, std::vector<const void*>& input, std::vector<void*>& output) override;

    virtual void forecast(int noutput, int& ninput_required) override;

};

#endif //SIMPLATFORM_NULLSOURCE_H
