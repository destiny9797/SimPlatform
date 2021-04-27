//
// Created by zhujiaying on 2021/4/27.
//

#include "../VectorCalc.h"
#include <vector>
#include <time.h>
#include <iostream>
#include <string>

void test_f_optimize(unsigned N)
{

    std::vector<float> x(N);
    std::vector<float> y(N);
    for (int i=0; i<N; ++i)
    {
        x[i] = i;
        y[i] = 2*i;
    }

    float result = 0;
    clock_t start,finish;
    double duration;

    start = clock();
    f_prod_f(&result,&x[0],&y[0],N);
    finish = clock();

    duration = (double)(finish-start)/CLOCKS_PER_SEC;
    std::cout << "test f_optimize, reuslt=" << result << ", time=" << 1000*duration << "ms" << std::endl;

}

void test_f_normal(unsigned N)
{
    std::vector<float> x(N);
    std::vector<float> y(N);
    for (int i=0; i<N; ++i)
    {
        x[i] = i;
        y[i] = 2*i;
    }

    clock_t start,finish;
    double duration;
    float result = 0;

    start = clock();
    for (int i=0; i<N; ++i)
    {
        result += x[i]*y[i];
    }
    finish = clock();

    duration = (double)(finish-start)/CLOCKS_PER_SEC;
    std::cout << "test f_normal, result=" << result << ", time=" << 1000*duration << "ms" << std::endl;

}

void test_fc_optimize(unsigned N)
{

    std::vector<std::complex<float>> x(N);
    std::vector<std::complex<float>> y(N);
    for (int i=0; i<N; ++i)
    {
        x[i] = i;
        y[i] = 2*i;
    }

    std::complex<float> result = 0;
    clock_t start,finish;
    double duration;

    start = clock();
    fc_prod_fc(&result,&x[0],&y[0],N);
    finish = clock();

    duration = (double)(finish-start)/CLOCKS_PER_SEC;
    std::cout << "test fc_optimize reuslt=" << result << ", time=" << 1000*duration << "ms" << std::endl;

}

void test_fc_normal(unsigned N)
{
    std::vector<std::complex<float>> x(N);
    std::vector<std::complex<float>> y(N);
    for (int i=0; i<N; ++i)
    {
        x[i] = i;
        y[i] = 2*i;
    }
    std::complex<float> result = 0;
    clock_t start,finish;
    double duration;

    start = clock();
    for (int i=0; i<N; ++i)
    {
        result += x[i]*y[i];
    }
    finish = clock();

    duration = (double)(finish-start)/CLOCKS_PER_SEC;
    std::cout << "test fc_normal, result=" << result << ", time=" << 1000*duration << "ms" << std::endl;

}

int main(int argc, const char * argv[])
{
    unsigned int N = 4960;
    if (argc<3)
    {
        std::cout << "usage: test_vectorcalc testname number_of_points, "
                     "testname canbe: f_normal, f_optimize, fc_normal, fc_optimize" << std::endl;
        return 0;
    }
    std::string testname = argv[1];
    N = atoi(argv[2]);


    if (testname == "f_normal")
    {
        test_f_normal(N);
    }
    else if (testname == "f_optimize")
    {
        test_f_optimize(N);
    }
    else if (testname == "fc_normal")
    {
        test_fc_normal(N);
    }
    else if (testname == "fc_optimize")
    {
        test_fc_optimize(N);
    }
    else
    {
        std::cerr << "wrong testname, testname canbe: f_normal, f_optimize, fc_normal, fc_optimize" << std::endl;
    }


    return 0;
}