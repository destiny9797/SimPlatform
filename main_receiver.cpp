//
// Created by zhujiaying on 2021/3/30.
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

    spBasicBlock sourceapi = std::make_shared<SourceInterface>("shared_memory", sizeof(std::complex<float>));

    spBasicBlock bpsk_demod = std::make_shared<BPSKDemod>();
    spBasicBlock bit_to_byte = std::make_shared<ConvertByteBit>(false);
    spBasicBlock sink = std::make_shared<NullSink>(1,sizeof(uint8_t),0,0);

    topflow.Connect(sourceapi,0,bpsk_demod,0);
    topflow.Connect(bpsk_demod,0,bit_to_byte,0);
    topflow.Connect(bit_to_byte,0,sink,0);

    topflow.Start();
    topflow.Run();
    topflow.Wait();

    return 0;
}