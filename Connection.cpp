//
// Created by zhujiaying on 2021/3/12.
//

#include "Connection.h"
#include <iostream>

#define NITEMS ( 8 * (1L<<10) ) //8M ，如果sizeofitem是4Bytes，则总共约32M一个文件

BlkPort::BlkPort(spBasicBlock blk, int port)
    : _block(blk),
      _port(port)
{

}

BlkPort::~BlkPort()
{
//    std::cout << "~BlkPort()" << std::endl;
}

Connection::Connection()
    : _connlist(),
      _outblocklist(),
      _inblocklist()
{

}

Connection::~Connection()
{
//    std::cout << "~Connection()" << std::endl;
}


void Connection::Register(Pair &apair)
{
    _connlist.emplace_back(apair);
}

void Connection::Unregister(Pair &apair)
{
    _connlist.pop_back();
}

BlkPort& Connection::GetUpStream(const BlkPort& blkport)
{
    for (Pair& p : _connlist)
    {
        if (p.second==blkport)
        {
            return p.first;
        }
    }
    BlkPort nullbp = BlkPort(nullptr,-1);
    return nullbp;
}

std::vector<BlkPort> Connection::GetDownStram(const BlkPort &blkport)
{
    std::vector<BlkPort> result;
    for (Pair& p : _connlist)
    {
        if (p.first==blkport)
        {
            result.emplace_back(p.second);
        }
    }
    return result;
}

std::vector<Connection::spBasicBlock> Connection::GetUsedBlocks()
{
    std::vector<spBasicBlock> tmp;
    for (Pair& p : _connlist)
    {
        tmp.push_back(p.first.GetBlock());
        tmp.push_back(p.second.GetBlock());
    }
    //对vector中的block去重
    std::sort(tmp.begin(),tmp.end());
    std::vector<spBasicBlock> result;
    std::unique_copy(tmp.begin(),tmp.end(),std::back_inserter(result));

    //拓扑排序
    std::vector<spBasicBlock> finalresult = TopologicSort(result);
    if (finalresult.empty())
    {
        throw std::runtime_error("Connection[GetUsedBlocks]: Wrong connection, has loop!");
    }

    return finalresult;
}

std::vector<Connection::spBasicBlock> Connection::TopologicSort(const std::vector<spBasicBlock>& blocks)
{
    std::unordered_map<spBasicBlock, std::unordered_set<spBasicBlock>> graph;
    std::unordered_map<spBasicBlock, int> degree;
    for (spBasicBlock block : blocks)
    {
        graph[block] = std::unordered_set<spBasicBlock>();
        degree[block] = 0;
    }

    for (Pair& p : _connlist)
    {
        BlkPort src = p.first;
        BlkPort dst = p.second;
        if (graph[src.GetBlock()].find(dst.GetBlock()) == graph[src.GetBlock()].end())
        {
            graph[src.GetBlock()].insert(dst.GetBlock());
            degree[dst.GetBlock()]++;
        }
    }

    std::queue<spBasicBlock> q;
    for (const std::pair<spBasicBlock,int>& d : degree)
    {
        if (d.second == 0)
        {
            q.push(d.first);
        }
    }

    std::vector<spBasicBlock> topo;
    while (!q.empty())
    {
        spBasicBlock block = q.front();
        q.pop();
        topo.emplace_back(block);
        for (spBasicBlock nextblock : graph[block])
        {
            degree[nextblock]--;
            if (degree[nextblock] == 0)
            {
                q.push(nextblock);
            }
        }
    }
    if (topo.size() != blocks.size())
    {
        topo.clear();
    }
    return topo;
}

bool Connection::CheckUsed(spBasicBlock blk, int port)
{
    for (Pair& p : _connlist)
    {
        if (p.second.GetBlock()==blk && p.second.GetPort()==port)
        {
            return true;
        }
    }
    return false;
}

void Connection::CheckConn()
{
    //检查各个端口是否都已连接
    for (Pair& p : _connlist)
    {
//        BlkPort outbp = p.first;
        spBasicBlock block = p.first.GetBlock();
        if (_outblocklist.count(block) == 0)
            _outblocklist.insert(block);
        if (block->GetInputPortNum()>0 && _inblocklist.count(block)==0)
            _inblocklist.insert(block);
        block = p.second.GetBlock();
        if (_inblocklist.count(block) == 0)
            _inblocklist.insert(block);
        if (block->GetOutputPortNum()>0 && _outblocklist.count(block)==0)
            _outblocklist.insert(block);

        //检查是否flow中每个模块的输出端口都已经连接到下一个block，否则不会为它分配buffer
        for (const spBasicBlock& outblock : _outblocklist)
        {
            int nport = outblock->GetInputPortNum();
            for (int port=0; port<nport; ++port)
            {
                if (GetDownStram(BlkPort(outblock,port)).empty())
                {
                    throw std::runtime_error("Connection[CheckConn]: an output port has no destination.");
                }
            }
        }

        //检查是否flow中每个模块的输入端口都已经连接
        for (const spBasicBlock& inblock : _inblocklist)
        {
            int nport = inblock->GetInputPortNum();
            for (int port=0; port<nport; ++port)
            {
                if (GetUpStream(BlkPort(inblock,port)).GetBlock() == nullptr)
                {
                    throw std::runtime_error("Connection[CheckConn]: an input port has no source.");
                }
            }
        }
    }

}

void Connection::SetupConn()
{
    //为每个输出端口分配buffer，如果输出到多处，也只有一个buffer，但是读多少由下一级block决定
//    for (const BlkPort& outbp : _outblocklist)
//    {
//        size_t sizeofitem = outbp.GetBlock()->GetOutputSizeofitem();
//        spBuffer buffer = std::make_shared<Buffer>(outbp.GetBlock(), NITEMS, sizeofitem);
//        outbp.GetBlock()->SetOutbuffer(outbp.GetPort(),buffer);
//    }
    for (const spBasicBlock& block: _outblocklist)
    {
        int sizeofitem = block->GetOutputSizeofitem();
        for (int i=0; i<block->GetOutputPortNum(); ++i)
        {
            spBuffer buffer = std::make_shared<Buffer>(block, NITEMS, sizeofitem);
            block->SetOutbuffer(i, buffer);
        }
    }

    //为每个输入端口，连接到上一级的对应输出buffer，对于同一个输出buffer，可能输出到多个口，会有多个读取指针，因此需要新建一个类
    for (Pair& p : _connlist)
    {
        BlkPort& inbp = p.second;
        const BlkPort& upbp = GetUpStream(inbp);
        spBuffer buffer = upbp.GetBlock()->GetOutbuffer(upbp.GetPort());
        spBufferReader buffer_reader = std::make_shared<BufferReader>(inbp.GetBlock(), buffer);
        buffer->AddReader(buffer_reader);
        inbp.GetBlock()->SetInbuffer(inbp.GetPort(),buffer_reader);
    }

}
