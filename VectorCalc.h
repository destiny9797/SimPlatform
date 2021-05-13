//
// Created by zhujiaying on 2021/4/27.
//

#ifndef SIMPLATFORM_VECTORCALC_H
#define SIMPLATFORM_VECTORCALC_H

#include <complex>

void f_prod_f(float* result, const float* x, const float* y, unsigned int N);

void f_prod_f2(float* result, const float* x, const float* y, unsigned int N);

void fc_prod_fc(std::complex<float>* result, const std::complex<float>* x, const std::complex<float>* y, unsigned int N);

#endif //SIMPLATFORM_VECTORCALC_H
