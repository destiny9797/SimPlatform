
project(SimPlatform)

set(CMAKE_CXX_STANDARD 14)

add_executable(test_bpsk
        test/test_bpsk.cpp
        TopFlow.cpp
        BasicBlock.cpp
        Connection.cpp
        Buffer.cpp
        Thread.cpp
        ThreadPool.cpp
        Interface.cpp
        TranState.cpp
        MsgControl.cpp
        TaskPool.cpp
        TaskType.cpp

        Blocks/NullSource.cpp
        Blocks/NullSink.cpp
        Blocks/MsgGenerator.cpp
        Blocks/ConvertByteBit.cpp
        Blocks/BPSK.cpp
        Blocks/BPSKDemod.cpp
        Blocks/AWGNChannel.cpp
        Blocks/BER.cpp
        Blocks/FrameEncap.cpp
        Blocks/FrameSync.cpp
        )

add_executable(test_framesync
        test/test_framesync.cpp
        TopFlow.cpp
        BasicBlock.cpp
        Connection.cpp
        Buffer.cpp
        Thread.cpp
        ThreadPool.cpp
        Interface.cpp
        TranState.cpp
        MsgControl.cpp
        TaskPool.cpp
        TaskType.cpp

        Blocks/NullSource.cpp
        Blocks/NullSink.cpp
        Blocks/MsgGenerator.cpp
        Blocks/ConvertByteBit.cpp
        Blocks/BPSK.cpp
        Blocks/BPSKDemod.cpp
        Blocks/FrameEncap.cpp
        Blocks/FrameSync.cpp
        )

add_executable(test_transmitter
        test/test_transmitter.cpp
        TopFlow.cpp
        BasicBlock.cpp
        Connection.cpp
        Buffer.cpp
        Thread.cpp
        ThreadPool.cpp
        Interface.cpp
        TranState.cpp
        MsgControl.cpp
        TaskPool.cpp
        TaskType.cpp

        Blocks/ConvertByteBit.cpp
        Blocks/BPSK.cpp
        Blocks/BPSKDemod.cpp
        Blocks/FrameEncap.cpp
        Blocks/FrameSync.cpp
        )

add_executable(test_receiver
        test/test_receiver.cpp
        TopFlow.cpp
        BasicBlock.cpp
        Connection.cpp
        Buffer.cpp
        Thread.cpp
        ThreadPool.cpp
        Interface.cpp
        TranState.cpp
        MsgControl.cpp
        TaskPool.cpp
        TaskType.cpp

        Blocks/ConvertByteBit.cpp
        Blocks/BPSK.cpp
        Blocks/BPSKDemod.cpp
        Blocks/FrameEncap.cpp
        Blocks/FrameSync.cpp
        )

add_executable(test_transmitter
        test/test_transmitter.cpp
        TopFlow.cpp
        BasicBlock.cpp
        Connection.cpp
        Buffer.cpp
        Thread.cpp
        ThreadPool.cpp
        Interface.cpp
        TranState.cpp
        MsgControl.cpp
        TaskPool.cpp
        TaskType.cpp

        Blocks/ConvertByteBit.cpp
        Blocks/BPSK.cpp
        Blocks/BPSKDemod.cpp
        Blocks/FrameEncap.cpp
        Blocks/FrameSync.cpp
        Blocks/Multiply.cpp
        Blocks/SRRCFilter.h

        SourceBlocks/fxpt.cpp
        )

add_executable(Tx
        test/Tx.cpp
        TopFlow.cpp
        BasicBlock.cpp
        Connection.cpp
        Buffer.cpp
        Thread.cpp
        ThreadPool.cpp
        Interface.cpp
        TranState.cpp
        MsgControl.cpp
        TaskPool.cpp
        TaskType.cpp

        Blocks/ConvertByteBit.cpp
        Blocks/BPSK.cpp
        Blocks/BPSKDemod.cpp
        Blocks/FrameEncap.cpp
        Blocks/FrameSync.cpp
        Blocks/Multiply.cpp
        Blocks/SRRCFilter.cpp
        Blocks/FirFilter.cpp

        SourceBlocks/fxpt.cpp
        )

add_executable(Rx
        test/Rx.cpp
        TopFlow.cpp
        BasicBlock.cpp
        Connection.cpp
        Buffer.cpp
        Thread.cpp
        ThreadPool.cpp
        Interface.cpp
        TranState.cpp
        MsgControl.cpp
        TaskPool.cpp
        TaskType.cpp

        Blocks/NullSink.cpp
        Blocks/FirFilter.cpp
        Blocks/BPSKDemod.cpp
        Blocks/ConvertByteBit.cpp
        Blocks/Decimator.cpp
        Blocks/BitsyncMM.cpp
        Blocks/FrameSync.cpp

        SourceBlocks/fxpt.cpp
        )

add_executable(test_vectorcalc
        test/test_vectorcalc.cpp
        VectorCalc.cpp)