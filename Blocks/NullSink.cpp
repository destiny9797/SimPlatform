//
// Created by zhujiaying on 2021/3/15.
//

#include "NullSink.h"
#include <iostream>
#include <complex>

static int num = 1;

NullSink::NullSink(int ninput, int sizeofinitem, int noutput, int sizeofoutitem)
    : BasicBlock(ninput, sizeofinitem, noutput, sizeofoutitem)
{
}

NullSink::~NullSink() noexcept
{
//    std::cout << "~NullSink()" << std::endl;
}

void NullSink::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput / GetInterpolation() * GetDecimation() + GetHistory() - 1;
}

int NullSink::work(int noutput, std::vector<const void *> &input, std::vector<void *> &output)
{

//    std::cout << "a work : " << noutput << std::endl;
    const uint8_t* in = (const uint8_t*)input[0];

//    int k = 0;
//    for (int i=0; i<noutput; ++i)
//    {
//        int sum = 0;
//
//        for (int j=k; j<k+GetHistory(); ++j)
//        {
//            sum += in[j];
//        }
//        std::cout << sum << "," ;//<< in[2*i+1] << " | ";
//        k += GetDecimation();
//        if (num == 10)
//        {
//            std::cout << std::endl;
//            num -= 10;
//        }
//        num++;
//    }

//    std::cout << "thread: " << std::this_thread::get_id() << std::endl;

    for (int i=0; i<noutput; ++i)
    {

    }


    return noutput;
}