

#include "TopFlow.h"
#include "BasicBlock.h"
#include "Blocks/NullSource.h"
#include "Blocks/NullSink.h"
#include "Blocks/Proc.h"
#include "Sources/sig_source.h"
#include "Blocks/mi_channel.h"
#include "Blocks/node_tag.h"
#include "Blocks/MsgGenerator.h"
#include "Blocks/ConvertByteBit.h"
#include "Blocks/BPSK.h"
#include "Blocks/BPSKDemod.h"
#include "Blocks/AWGNChannel.h"
#include "Blocks/BER.h"
#include <complex>
#include <ctime>
#include <iostream>

typedef std::shared_ptr<BasicBlock> spBasicBlock;
typedef std::shared_ptr<node_tag> spNode;

typedef std::complex<float> lv_32fc_t;

void volk_32fc_x2_dot_prod_32fc_generic(lv_32fc_t* result,
                                      const lv_32fc_t* input,
                                      const lv_32fc_t* taps,
                                      unsigned int num_points)
{

    float* res = (float*)result;
    float* in = (float*)input;
    float* tp = (float*)taps;
    unsigned int n_2_ccomplex_blocks = num_points / 2;

    float sum0[2] = { 0, 0 };
    float sum1[2] = { 0, 0 };
    unsigned int i = 0;

    for (i = 0; i < n_2_ccomplex_blocks; ++i) {
        sum0[0] += in[0] * tp[0] - in[1] * tp[1];
        sum0[1] += in[0] * tp[1] + in[1] * tp[0];
        sum1[0] += in[2] * tp[2] - in[3] * tp[3];
        sum1[1] += in[2] * tp[3] + in[3] * tp[2];

        in += 4;
        tp += 4;
    }

    res[0] = sum0[0] + sum1[0];
    res[1] = sum0[1] + sum1[1];

    // Cleanup if we had an odd number of points
    if (num_points & 1) {
        *result += input[num_points - 1] * taps[num_points - 1];
    }
}

void compute_optimize(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_points)
{
    float* res = (float*)result;
    float* in = (float*)input;
    float* tp = (float*)taps;

    float sum0[2] = {0,0};
    unsigned int i = 0;
    for (i=0; i<num_points; ++i)
    {
        sum0[0] += in[0] * tp[0] - in[1] * tp[1];
        sum0[1] += in[0] * tp[1] + in[1] * tp[0];
        in += 2;
        tp += 2;
    }
    res[0] = sum0[0];
    res[1] = sum0[1];
}

void test_compute_optimize()
{
    const unsigned int num = 4096;
    lv_32fc_t input[num];
    lv_32fc_t taps[num];
    lv_32fc_t result;
    for (int i=0; i<num; ++i)
    {
        input[i] = {(float)(i%5)/(float)5.0, (float)(i%7)/(float)6.0};
        taps[i] = {(float)(i%10)/(float)3.0, (float)(i%8)/(float)8.0};
    }

    clock_t start,end;
    start = clock();
    compute_optimize(&result, input, taps, num);
    end = clock();
    double normaltime = (double)(end-start)/CLOCKS_PER_SEC;
    std::cout << "normal result: " << result << std::endl;
    std::cout << "normal time: " << normaltime*1000 << "ms" << std::endl;

    start = clock();
    volk_32fc_x2_dot_prod_32fc_generic(&result, input, taps, num);
    end = clock();
    double volktime = (double)(end-start)/CLOCKS_PER_SEC;
    std::cout << "volk result: " << result << std::endl;
    std::cout << "volk time: " << volktime*1000 << "ms" << std::endl;
}

void tests()
{
    float snr = 0;
    float En = 1.0 / pow(10.0, snr / 10.0);

    TopFlow topflow;

    spBasicBlock msg = std::make_shared<MsgGenerator>(100000);
    spBasicBlock byte_to_bit = std::make_shared<ConvertByteBit>();
    spBasicBlock bpsk_mod = std::make_shared<BPSK>();
    spBasicBlock channel = std::make_shared<AWGNChannel>(En);
    spBasicBlock bpsk_demod = std::make_shared<BPSKDemod>();
    spBasicBlock bit_to_byte = std::make_shared<ConvertByteBit>(false);
    spBasicBlock sink = std::make_shared<NullSink>(1,sizeof(uint8_t),0,0);
    spBasicBlock ber = std::make_shared<BER>();

    topflow.Connect(msg,0,byte_to_bit,0);
    topflow.Connect(byte_to_bit,0,bpsk_mod,0);
    topflow.Connect(bpsk_mod,0,channel,0);
    topflow.Connect(channel,0,bpsk_demod,0);
    topflow.Connect(bpsk_demod,0,bit_to_byte,0);
    topflow.Connect(bit_to_byte,0,sink,0);
    topflow.Connect(byte_to_bit,0,ber,0);
    topflow.Connect(bpsk_demod,0,ber,1);

    topflow.Start();
    topflow.Run();
    topflow.Wait();

}

int main(int argc, char* argv[])
{
    tests();

    int i = 1;

    return 0;
}
