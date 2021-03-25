//
// Created by zhujiaying on 2021/3/13.
//

#include "NullSource.h"
#include <iostream>

static int num = 0;

NullSource::NullSource(int ninput, int sizeofinitem, int noutput, int sizeofoutitem)
    : BasicBlock(ninput,sizeofinitem,noutput,sizeofoutitem)
{

}

NullSource::~NullSource() noexcept
{
    std::cout << "~NullSource()" << std::endl;
}

void NullSource::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput + GetHistory() - 1;
}

int NullSource::work(int noutput, std::vector<const void *> &input, std::vector<void *> &output)
{
    if (num == 100000)
        return -1;

    int* out = (int*)output[0];

    int i=0;
    for (i=0; i<noutput && num<100000; ++i)
    {
//        for (int j=0; j<10000; ++j){}
        out[i] = (num++) ;
    }

    return i;

}