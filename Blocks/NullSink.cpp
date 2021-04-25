//
// Created by zhujiaying on 2021/3/15.
//

#include "NullSink.h"
#include <iostream>
#include <complex>

static int num = 1;

template <class T>
NullSink<T>::NullSink(std::string name)
    : BasicBlock(name, 1, sizeof(T), 0, 0)
{
}

template <class T>
NullSink<T>::~NullSink()
{
//    std::cout << "~NullSink()" << std::endl;
}

template <class T>
void NullSink<T>::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput / GetInterpolation() * GetDecimation() + GetHistory() - 1;
}

template <class T>
int NullSink<T>::work(int noutput, int& ninput, std::vector<const void *> &input, std::vector<void *> &output)
{

//    std::cout << "a work : " << noutput << std::endl;
    const T* in = (const T*)input[0];

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

    std::unique_lock<std::mutex> lk(BasicBlock::_blockmutex);

//    std::cout << "thread: " << std::this_thread::get_id() << std::endl;

    for (int i=0; i<noutput; ++i)
    {

        std::cout << in[i] << " ";
//        num++;
//        if (num == 96)
//        {
//            std::cout << std::endl;
//            num = 0;
//        }
    }
//    std::cout << std::endl;

    ninput = noutput;
    return noutput;
}