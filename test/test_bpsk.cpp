//
// Created by zhujiaying on 2021/4/1.
//

#include "../TopFlow.h"
#include "../BasicBlock.h"
#include "../Blocks/NullSink.cpp"
#include "../Blocks/MsgGenerator.h"
#include "../Blocks/ConvertByteBit.h"
#include "../Blocks/BPSK.h"
#include "../Blocks/BPSKDemod.h"
#include "../Blocks/AWGNChannel.h"
#include "../Blocks/BER.h"

typedef std::shared_ptr<BasicBlock> spBasicBlock;

void test1(float snr, int nbytes)
{
    //测试BPSK调制的性能，信噪比为3dB，共100k字节(800kbit)的数据
//    float snr = 5;
    float En = 1.0 / pow(10.0, snr / 10.0);

    TopFlow topflow;

    spBasicBlock msg = std::make_shared<MsgGenerator>("msg",nbytes);
    spBasicBlock byte_to_bit = std::make_shared<ConvertByteBit>("byte_to_bit");
    spBasicBlock bpsk_mod = std::make_shared<BPSK>("bpsk_mod");
    spBasicBlock channel = std::make_shared<AWGNChannel<float>>("channel",En);
    spBasicBlock bpsk_demod = std::make_shared<BPSKDemod>("bpsk_demod");
    spBasicBlock bit_to_byte = std::make_shared<ConvertByteBit>("bit_to_byte",false);
    spBasicBlock sink = std::make_shared<NullSink<uint8_t>>("sink");
    spBasicBlock ber = std::make_shared<BER>("ber");

    topflow.Connect(msg,0,byte_to_bit,0);
    topflow.Connect(byte_to_bit,0,bpsk_mod,0);
    topflow.Connect(bpsk_mod,0,channel,0);
    topflow.Connect(channel,0,bpsk_demod,0);
    topflow.Connect(byte_to_bit,0,ber,0);
    topflow.Connect(bpsk_demod,0,ber,1);

    topflow.Start();
    topflow.Run();
    topflow.Wait();

}

int main(int argc, char* argv[])
{
    if (argc<3){
        std::cout << "usage: test_bpsk SNR number_of_bytes" << std::endl;
        return 0;
    }
    float snr = atof(argv[1]);
    int nbytes = atoi(argv[2]);

    test1(snr,nbytes);

    return 0;
}