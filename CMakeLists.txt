
project(SimPlatform)

set(CMAKE_CXX_STANDARD 14)

add_executable(SimPlatform
        main.cpp
        TopFlow.cpp
        BasicBlock.cpp
        Connection.cpp
        Buffer.cpp
        Thread.cpp
        ThreadPool.cpp

        Blocks/NullSource.cpp
        Blocks/NullSink.cpp
        Blocks/Proc.cpp
        Blocks/mi_channel.cpp
        Blocks/node_tag.cpp
        Blocks/MsgGenerator.cpp
        Blocks/ConvertByteBit.cpp
        Blocks/BPSK.cpp
        Blocks/BPSKDemod.cpp
        Blocks/AWGNChannel.cpp
        Blocks/BER.cpp

        Sources/fxpt.cc

        )