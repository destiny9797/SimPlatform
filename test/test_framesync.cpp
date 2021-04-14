//
// Created by zhujiaying on 2021/4/13.
//

#include "../TopFlow.h"
#include "../BasicBlock.h"
#include "../Blocks/NullSink.h"
#include "../Blocks/MsgGenerator.h"
#include "../Blocks/ConvertByteBit.h"
#include "../Blocks/BPSK.h"
#include "../Blocks/BPSKDemod.h"
#include "../Blocks/FrameEncap.h"
#include "../Blocks/FrameSync.h"
#include <complex>

typedef std::shared_ptr<BasicBlock> spBasicBlock;

void test1()
{
    TopFlow topflow;

    spBasicBlock msg = std::make_shared<MsgGenerator>("msg",13);
    spBasicBlock byte_to_bit = std::make_shared<ConvertByteBit>("byte_to_bit");
    spBasicBlock frameencap = std::make_shared<FrameEncap>("header","1111100110101", 96);
    spBasicBlock bpsk_mod = std::make_shared<BPSK>("bpsk_mod");
    spBasicBlock bpsk_demod = std::make_shared<BPSKDemod>("bpsk_demod");
    spBasicBlock framesync = std::make_shared<FrameSync>("framesync","1111100110101");
    spBasicBlock sink = std::make_shared<NullSink>("sink",1,sizeof(uint8_t),0,0);

    topflow.Connect(msg,0,byte_to_bit,0);
    topflow.Connect(byte_to_bit,0,frameencap,0);
    topflow.Connect(frameencap,0,bpsk_mod,0);
    topflow.Connect(bpsk_mod,0,bpsk_demod,0);
    topflow.Connect(bpsk_demod,0,framesync,0);
    topflow.Connect(framesync,0,sink,0);

    topflow.Start();
    topflow.Run();
    topflow.Wait();

}

int main(int argc, char* argv[])
{
    test1();

    return 0;
}