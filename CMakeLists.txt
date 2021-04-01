
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
        )