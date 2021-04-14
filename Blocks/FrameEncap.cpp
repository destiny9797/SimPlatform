//
// Created by zhujiaying on 2021/4/13.
//

#include "FrameEncap.h"
#include <iostream>

FrameEncap::FrameEncap(std::string name, std::string header, int framelen)
    : BasicBlock(name, 1, sizeof(uint8_t), 1, sizeof(uint8_t)),
      _sequence(header),
      _framelen(framelen),
      _headerlen(header.size()),
      _countbit(framelen),
      _countheader(0)
{

}

FrameEncap::~FrameEncap()
{

}

void FrameEncap::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

int FrameEncap::work(int noutput, int& ninput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const uint8_t* in = (const uint8_t*)input[0];
    uint8_t* out = (uint8_t*)output[0];


//    std::unique_lock<std::mutex> lk(BasicBlock::_blockmutex);
//
//    std::cout << "thread: " << std::this_thread::get_id() << std::endl;

    int j = 0;
    for (int i=0; i<noutput; ++i)
    {
        if (_countbit == _framelen)
        {
            out[i] = _sequence[_countheader] - '0';
            ++_countheader;
            if (_countheader == _headerlen)
            {
                _countheader = 0;
                _countbit = 0;
//                std::cout << std::endl;
            }
        }
        else
        {
            out[i] = in[j];
            ++j;
            ++_countbit;
        }
//        std::cout << (int)out[i] << " ";
    }

    ninput = j;
    return noutput;
}