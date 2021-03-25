//
// Created by zhujiaying on 2021/3/12.
//

#include "TopFlow.h"
#include "BasicBlock.h"
#include "Blocks/NullSink.h"
#include "Blocks/MsgGenerator.h"
#include "Blocks/ConvertByteBit.h"
#include "Blocks/BPSK.h"
#include "Blocks/BPSKDemod.h"
#include "Blocks/AWGNChannel.h"
#include "Blocks/BER.h"

typedef std::shared_ptr<BasicBlock> spBasicBlock;

void test1()
{
    float snr = 5;
    float En = 1.0 / pow(10.0, snr / 10.0);

    TopFlow topflow;

    //测试BPSK调制的性能，信噪比为3dB，共100k字节(800kbit)的数据
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
    test1();

    return 0;
}
