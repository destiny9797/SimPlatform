//
// Created by zhujiaying on 2021/3/16.
//

#include "Proc.h"
#include <iostream>

static long long count = 0;

Proc::Proc(int ninput, int sizeofinitem, int noutput, int sizeofoutitem)
    : BasicBlock(ninput,sizeofinitem,noutput,sizeofoutitem)
{
//    SetHistory(2);
//    SetInterpolation(2);
}

Proc::~Proc() noexcept
{

}

void Proc::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput / GetInterpolation() * GetDecimation() + GetHistory() - 1;
}

int Proc::work(int noutput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const int* in = (const int*)input[0];
    int* out = (int*)output[0];

    int k = 0;
    for (int i=0; i<noutput; ++i)
    {
        for (int j=0; j<10000; ++j)
        {}
        out[i] = in[i];
//        int sum = 0;
//        for (int j=k; j<k+GetHistory(); ++j)
//        {
//            sum += in[j];
//        }
//        out[i] = sum;
//        if (i % GetInterpolation() == GetInterpolation() - 1)
//        {
//            k += 1;
//        }
    }
//    std::cout << std::endl;

    return noutput;
}




//计算优化
//static inline void volk_32fc_x2_dot_prod_32fc_generic(lv_32fc_t* result,
//                                                      const lv_32fc_t* input,
//                                                      const lv_32fc_t* taps,
//                                                      unsigned int num_points)
//{
//
//    float* res = (float*)result;
//    float* in = (float*)input;
//    float* tp = (float*)taps;
//    unsigned int n_2_ccomplex_blocks = num_points / 2;
//
//    float sum0[2] = { 0, 0 };
//    float sum1[2] = { 0, 0 };
//    unsigned int i = 0;
//
//    for (i = 0; i < n_2_ccomplex_blocks; ++i) {
//        sum0[0] += in[0] * tp[0] - in[1] * tp[1];
//        sum0[1] += in[0] * tp[1] + in[1] * tp[0];
//        sum1[0] += in[2] * tp[2] - in[3] * tp[3];
//        sum1[1] += in[2] * tp[3] + in[3] * tp[2];
//
//        in += 4;
//        tp += 4;
//    }
//
//    res[0] = sum0[0] + sum1[0];
//    res[1] = sum0[1] + sum1[1];
//
//    // Cleanup if we had an odd number of points
//    if (num_points & 1) {
//        *result += input[num_points - 1] * taps[num_points - 1];
//    }
//}