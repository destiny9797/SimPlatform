//
// Created by zhujiaying on 2021/3/24.
//

#include "BER.h"
#include <iostream>

BER::BER(std::string name)
    : BasicBlock(name, 2,sizeof(uint8_t),0,0),
      _ntotalbit(0),
      _nerrorbit(0)
{

}

BER::~BER() noexcept
{
    double ber = _nerrorbit==0 ? 0 : (double)_nerrorbit/(double)_ntotalbit;
    std::cout << "totalbit is: " << _ntotalbit << std::endl;
    std::cout << "Bit Error Rate is: " << ber << std::endl;
}

void BER::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

int BER::work(int noutput, int& ninput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const uint8_t* in1 = (const uint8_t*)input[0];
    const uint8_t* in2 = (const uint8_t*)input[1];

    for (int i=0; i<noutput; ++i)
    {
        ++_ntotalbit;
        if (in1[i] != in2[i])
            ++_nerrorbit;
    }

    ninput = noutput;
    return noutput;
}
