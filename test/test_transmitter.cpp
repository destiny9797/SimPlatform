//
// Created by zhujiaying on 2021/3/12.
//

#include "../TopFlow.h"
#include "../TranState.h"
#include "../BasicBlock.h"
#include "../Interface.h"
#include "../MsgControl.h"
#include "../TaskType.h"
#include "../Blocks/ConvertByteBit.h"
#include "../Blocks/FrameEncap.h"
#include "../Blocks/FrameSync.h"
#include "../Blocks/BPSK.h"
#include "../Blocks/BPSKDemod.h"
#include <complex>

typedef std::shared_ptr<BasicBlock> spBasicBlock;

TopFlow topflow;

static void signal_handler(int sig_no)
{
    topflow.Stop();
//    topflow.Wait();
//    exit(0);
}

int main(int argc, char* argv[])
{
    signal(SIGINT,signal_handler);
    signal(SIGHUP,signal_handler);

    std::shared_ptr<TranState> spstate = std::make_shared<TranState>(TranState::SEND);

    spBasicBlock sinkapi = std::make_shared<SinkInterface>(
            spstate,
            "AtoB",
            sizeof(float),
            SinkInterface::AWGN,
            SinkInterface::Rayleigh);

    spBasicBlock sourceapi = std::make_shared<SourceInterface>(
            spstate,
            "BtoA",
            sizeof(float));


    spBasicBlock msg = std::make_shared<MsgGenerater>(spstate);
    spBasicBlock byte_to_bit = std::make_shared<ConvertByteBit>("byte_to_bit");
    spBasicBlock frameencap = std::make_shared<FrameEncap>("header","1111100110101", 96);
    spBasicBlock bpsk_mod = std::make_shared<BPSK>("bpsk_mod");

    spBasicBlock bpsk_demod = std::make_shared<BPSKDemod>("bpsk_demod");
    spBasicBlock bit_to_byte = std::make_shared<ConvertByteBit>("bit_to_byte",false);
    spBasicBlock msgparser = std::make_shared<MsgParser>(spstate);


    topflow.Connect(msg,0,byte_to_bit,0);
    topflow.Connect(byte_to_bit,0,frameencap,0);
    topflow.Connect(frameencap,0,bpsk_mod,0);
    topflow.Connect(bpsk_mod,0,sinkapi,0);

    topflow.Connect(sourceapi,0,bpsk_demod,0);
    topflow.Connect(bpsk_demod,0,bit_to_byte,0);
    topflow.Connect(bit_to_byte,0,msgparser,0);

    topflow.AddTask(randombytes);

    topflow.Start();
    topflow.Run();
    topflow.Wait();

    return 0;
}
