//
// Created by zhujiaying on 2021/4/13.
//

#ifndef SIMPLATFORM_FRAMESYNC_H
#define SIMPLATFORM_FRAMESYNC_H

#include "../BasicBlock.h"

class FrameSync : public BasicBlock
{
public:
    FrameSync(std::string name, std::string header);

    ~FrameSync() override;

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output) override;

    std::string _header;

    int _headerlen;

    int _nrshift;

    bool _findheader;
};

#endif //SIMPLATFORM_FRAMESYNC_H
