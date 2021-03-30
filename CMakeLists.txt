
project(SimPlatform)

set(CMAKE_CXX_STANDARD 14)

add_executable(Transmitter
        main_transmitter.cpp
        TopFlow.cpp
        BasicBlock.cpp
        Connection.cpp
        Buffer.cpp
        Thread.cpp
        ThreadPool.cpp
        Interface.cpp

        Blocks/NullSource.cpp
        Blocks/NullSink.cpp
        Blocks/MsgGenerator.cpp
        Blocks/ConvertByteBit.cpp
        Blocks/BPSK.cpp
        Blocks/BPSKDemod.cpp
        Blocks/AWGNChannel.cpp
        Blocks/BER.cpp
        )

add_executable(Receiver
        main_receiver.cpp
        TopFlow.cpp
        BasicBlock.cpp
        Connection.cpp
        Buffer.cpp
        Thread.cpp
        ThreadPool.cpp
        Interface.cpp

        Blocks/NullSource.cpp
        Blocks/NullSink.cpp
        Blocks/MsgGenerator.cpp
        Blocks/ConvertByteBit.cpp
        Blocks/BPSK.cpp
        Blocks/BPSKDemod.cpp
        Blocks/AWGNChannel.cpp
        Blocks/BER.cpp
        )