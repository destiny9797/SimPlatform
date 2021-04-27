//
// Created by zhujiaying on 2021/4/27.
//

#ifndef SIMPLATFORM_VECTORCALC_H
#define SIMPLATFORM_VECTORCALC_H

#include <complex>

void f_prod_f(float* result, const float* x, const float* y, unsigned int N)
{
    float doin2roduct = 0;
    const float* aPtr = x;
    const float* bPtr = y;
    unsigned int number = 0;

    for (number = 0; number < N; number++) {
        doin2roduct += ((*aPtr++) * (*bPtr++));
    }

    *result = doin2roduct;
}

void f_prod_f2(float* result, const float* x, const float* y, unsigned int N)
{
    float doin2roduct = 0;
    const float* aPtr = x;
    const float* bPtr = y;
    unsigned int number = 0;
    int n = N/2;

    for (number = 0; number < n; number++) {
        doin2roduct += ((*aPtr++) * (*bPtr++));
        doin2roduct += ((*aPtr++) * (*bPtr++));
    }
    if (N & 1)
    {
        doin2roduct += ((*aPtr++) * (*bPtr++));
    }

    *result = doin2roduct;
}

void fc_prod_fc(std::complex<float>* result, const std::complex<float>* x, const std::complex<float>* y, unsigned int N)
{
    float* res = (float*)result;
    float* in1 = (float*)x;
    float* in2 = (float*)x;
    unsigned int n = N / 2;

    float sum0[2] = { 0, 0 };
    float sum1[2] = { 0, 0 };
    unsigned int i = 0;

    for (i = 0; i < n; ++i) {
        sum0[0] += in1[0] * in2[0] - in1[1] * in2[1];
        sum0[1] += in1[0] * in2[1] + in1[1] * in2[0];
        sum1[0] += in1[2] * in2[2] - in1[3] * in2[3];
        sum1[1] += in1[2] * in2[3] + in1[3] * in2[2];

        in1 += 4;
        in2 += 4;
    }

    res[0] = sum0[0] + sum1[0];
    res[1] = sum0[1] + sum1[1];

    // Cleanup if we had an odd number of points
    if (N & 1) {
        *result += x[N - 1] * y[N - 1];
    }
}

#endif //SIMPLATFORM_VECTORCALC_H
