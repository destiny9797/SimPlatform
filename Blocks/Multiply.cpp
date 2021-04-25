//
// Created by zhujiaying on 2021/4/21.
//

#include "Multiply.h"

template <class T>
Multiply<T>::Multiply(std::string name)
    : BasicBlock(name,2,sizeof(T),1,sizeof(T))
{

}

template <class T>
Multiply<T>::~Multiply()
{

}

template <class T>
void Multiply<T>::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput;
}

template <class T>
int Multiply<T>::work(int noutput, int &ninput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const T* in1 = (const T*)input[0];
    const T* in2 = (const T*)input[1];
    T* out = (T*)output[0];

    for (int i=0; i<noutput; ++i)
    {
        out[i] = in1[i]*in2[i];
    }

    ninput = noutput;
    return noutput;

}
