//
// Created by zhujiaying on 2021/3/12.
//

#include "TopFlow.h"
#include "BasicBlock.h"
#include "Interface.h"
#include "Blocks/NullSink.h"
#include "Blocks/MsgGenerator.h"
#include "Blocks/ConvertByteBit.h"
#include "Blocks/BPSK.h"
#include "Blocks/BPSKDemod.h"
#include "Blocks/AWGNChannel.h"
#include "Blocks/BER.h"

typedef std::shared_ptr<BasicBlock> spBasicBlock;

TopFlow topflow;

static void signal_handler(int sig_no)
{
    topflow.Stop();
}

int main(int argc, char* argv[])
{
    signal(SIGINT,signal_handler);

    spBasicBlock msg = std::make_shared<MsgGenerator>(200000000);
    spBasicBlock byte_to_bit = std::make_shared<ConvertByteBit>();
    spBasicBlock bpsk_mod = std::make_shared<BPSK>();

    spBasicBlock sinkapi = std::make_shared<SinkInterface>("shared_memory", sizeof(std::complex<float>),SinkInterface::AWGN,SinkInterface::Rayleigh);


    topflow.Connect(msg,0,byte_to_bit,0);
    topflow.Connect(byte_to_bit,0,bpsk_mod,0);
    topflow.Connect(bpsk_mod,0,sinkapi,0);

    topflow.Start();
    topflow.Run();
    topflow.Wait();

    return 0;
}
