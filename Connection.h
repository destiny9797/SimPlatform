//
// Created by zhujiaying on 2021/3/12.
//

#ifndef SIMPLATFORM_CONNECTION_H
#define SIMPLATFORM_CONNECTION_H

#include "BasicBlock.h"
#include "Buffer.h"
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>

class BlkPort
{
public:
    typedef std::shared_ptr<BasicBlock> spBasicBlock;
    typedef std::shared_ptr<Buffer> spBuffer;
    typedef std::shared_ptr<BufferReader> spBufferReader;
    friend class Connection;
    BlkPort() : _block(nullptr), _port(-1) {}
    BlkPort(spBasicBlock blk, int port);
    ~BlkPort();

//    BasicBlock* GetBlock(){ return _block; }
    spBasicBlock GetBlock() const { return _block; }
    int GetPort() const { return _port; }

    bool operator==(const BlkPort& blkport)
    {
        return this->_block==blkport.GetBlock() && this->_port==blkport.GetPort();
    }


private:

    spBasicBlock _block;
    int _port;

};


class Connection
{
public:
    typedef std::pair<BlkPort,BlkPort> Pair;
    typedef BlkPort::spBasicBlock spBasicBlock;
    typedef BlkPort::spBuffer spBuffer;
    typedef BlkPort::spBufferReader  spBufferReader;
//    typedef std::shared_ptr<BlkPort> spBlkPort;
    Connection();
    ~Connection();

    void Register(Pair& apair);
    void Unregister(Pair& apair);

    int GetPairNum() { return _connlist.size(); }

    BlkPort& GetUpStream(const BlkPort& blkport);

    std::vector<BlkPort> GetDownStram(const BlkPort& blkPort);

    std::vector<spBasicBlock> GetUsedBlocks();

    bool CheckUsed(spBasicBlock blk, int port);

    //检查连接的正确性、完整性
    void CheckConn();

    //为流程中的模块分配buffer
    void SetupConn();

private:
    //拓扑排序，按照连接顺序，把每对连接的src放在dst前面
    std::vector<spBasicBlock> TopologicSort(const std::vector<spBasicBlock>& blocks);

    std::vector<Pair> _connlist;

    std::unordered_set<spBasicBlock> _outblocklist;
    std::unordered_set<spBasicBlock> _inblocklist;


};

#endif //SIMPLATFORM_CONNECTION_H
