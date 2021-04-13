//
// Created by zhujiaying on 2021/4/13.
//

#ifndef SIMPLATFORM_FRAMEENCAP_H
#define SIMPLATFORM_FRAMEENCAP_H

#include "../BasicBlock.h"

class FrameEncap : public BasicBlock
{
public:
    FrameEncap(std::string name, std::string header, int framelen);

    ~FrameEncap() override;

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output) override;

    std::string _sequence;

    //frame length without head
    int _framelen;

    int _headerlen;

    int _countbit;

    int _countheader;
};

#endif //SIMPLATFORM_FRAMEENCAP_H
