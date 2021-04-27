//
// Created by zhujiaying on 2021/4/25.
//


#include "filter_taps.h"
#include "../TopFlow.h"
#include "../BasicBlock.h"
#include "../Interface.h"
#include "../MsgControl.h"
#include "../TaskType.h"
#include "../Blocks/ConvertByteBit.h"
#include "../Blocks/BPSKDemod.h"
#include "../Sources/sig_source.h"
#include "../Blocks/Multiply.cpp"
#include "../Blocks/NullSink.cpp"
#include "../Blocks/FirFilter.h"
#include "../Blocks/Decimator.h"
#include "../Blocks/BitsyncMM.h"
#include "../Blocks/FrameSync.h"
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


    std::shared_ptr<TranState> spstate = std::make_shared<TranState>(TranState::RECEIVE);

    spBasicBlock sourceapi = std::make_shared<SourceInterface>(
            spstate,
            "AtoB",
            sizeof(float));

    spBasicBlock sine = std::make_shared<sig_source<float>>("sine",
                                                            80e3,
                                                            SIN_WAVE,
                                                            20e3,
                                                            1.0);

    spBasicBlock multiply = std::make_shared<Multiply<float>>("multiply");

    spBasicBlock lowpass = std::make_shared<FirFilter>("lowpass",lowpass_32_fs80000_fc10000);

    spBasicBlock decimator1 = std::make_shared<Decimator>("decimator1",4,lowpass_64_fs80000_fc5000);

    spBasicBlock decimator2 = std::make_shared<Decimator>("decimator2",5,lowpass_64_fs20000_fc1200);

    spBasicBlock bitsync = std::make_shared<BitsyncMM>("bitsync",4,0.25*0.175*0.175,0.5,0.175,0.005);

    spBasicBlock bpsk_demod = std::make_shared<BPSKDemod>("bpsk_demod");

    spBasicBlock framesync = std::make_shared<FrameSync>("framesync","1111100110101");

    spBasicBlock bit_to_byte = std::make_shared<ConvertByteBit>("bit_to_byte",false);

    spBasicBlock nullsink = std::make_shared<NullSink<uint8_t>>("nullsink");



    topflow.Connect(sourceapi,0,multiply,0);
    topflow.Connect(sine,0,multiply,1);
    topflow.Connect(multiply,0,lowpass,0);
    topflow.Connect(lowpass,0,decimator1,0);
    topflow.Connect(decimator1,0,decimator2,0);
    topflow.Connect(decimator2,0,bitsync,0);
    topflow.Connect(bitsync,0,bpsk_demod,0);
    topflow.Connect(bpsk_demod,0,framesync,0);
    topflow.Connect(framesync,0,nullsink,0);

//    topflow.AddTask(randombytes);

    topflow.Start();
    topflow.Run();
    topflow.Wait();

    return 0;
}
