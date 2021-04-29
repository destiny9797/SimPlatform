//
// Created by zhujiaying on 2021/4/21.
//

#include "../TopFlow.h"
#include "../TranState.h"
#include "../BasicBlock.h"
#include "../Interface.h"
#include "../MsgControl.h"
#include "../TaskType.h"
#include "../Blocks/BPSK.h"
#include "../Blocks/ConvertByteBit.h"
#include "../Blocks/FrameEncap.h"
#include "../Sources/sig_source.h"
#include "../Blocks/Multiply.cpp"
#include "../Blocks/SRRCFilter.h"
#include "../Blocks/FirFilter.h"
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

    std::string fname = "ssmm";
    std::string myname = "A";
    std::string partnername = "B";

    if (argc > 1)
    {
        fname = argv[1];
    }
    if (argc > 3)
    {
        myname = argv[2];
        partnername = argv[3];
    }

//    std::shared_ptr<TranState> spstate = std::make_shared<TranState>(TranState::SEND);
//
//    spBasicBlock sinkapi = std::make_shared<SinkInterface>(
//            spstate,
//            "AtoB",
//            sizeof(float));
    MakeInterface api(Interface::Tx,fname,myname,partnername,sizeof(float));

    spBasicBlock msg = std::make_shared<MsgGenerater>();
    spBasicBlock byte_to_bit = std::make_shared<ConvertByteBit>("byte_to_bit");
    spBasicBlock frameencap = std::make_shared<FrameEncap>("header","1111100110101", 96);
    spBasicBlock bpsk_mod = std::make_shared<BPSK>("bpsk_mod");
    spBasicBlock srrc = std::make_shared<SRRCFilter>("srrc",
                                                     80,
                                                     0.05,
                                                     15,
                                                     80);
//    spBasicBlock filter = std::make_shared<FirFilter>("srrc",srrctaps);
    spBasicBlock sine = std::make_shared<sig_source<float>>("sine",
                                                            80e3,
                                                            SIN_WAVE,
                                                            20e3,
                                                            1.0);
    spBasicBlock multiply = std::make_shared<Multiply<float>>("multiply");


    topflow.Connect(msg,0,byte_to_bit,0);
    topflow.Connect(byte_to_bit,0,frameencap,0);
    topflow.Connect(frameencap,0,bpsk_mod,0);
    topflow.Connect(bpsk_mod,0,srrc,0);
    topflow.Connect(srrc,0,multiply,0);
    topflow.Connect(sine,0,multiply,1);
    topflow.Connect(multiply,0,api.GetSink(),0);


    topflow.AddTask(randombytes);

    topflow.Start();
    topflow.Run();
    topflow.Wait();

    return 0;
}
