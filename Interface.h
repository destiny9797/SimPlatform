//
// Created by zhujiaying on 2021/3/29.
//

#ifndef SIMPLATFORM_INTERFACE_H
#define SIMPLATFORM_INTERFACE_H

#include "BasicBlock.h"
#include "TranState.h"
#include <string>
#include <iostream>

class MakeInterface;

class Interface : virtual public BasicBlock
{
public:
    friend class MakeInterface;

    typedef std::shared_ptr<TranState> spTranState;

    enum CommMode{
        Tx,
        Rx,
        TxRx_HalfD,
        TxRx_FullD
    };

    Interface(std::string fname, int sizeofitem, CommMode mode);

    ~Interface();


    int CalcAvailData()
    {
        return MinusInd(*_writeind, *_readind);
    }
    int CalcAvailSpace()
    {
        return _bufsize - CalcAvailData() - 1;
    }

    void SetDone()
    {
        *_done1 = 1;
    }

    int isDone()
    {
        return *_done1;
    }

    bool NoTx()
    {
        return _mode == Rx;
    }

    bool NoRx()
    {
        return _mode == Tx;
    }

protected:

    int AddInd(int x, int y)
    {
        int res = x + y;
        if (res >= _bufsize)
            res -= _bufsize;
        assert(res>=0 && res<_bufsize);
        return res;
    }

    int MinusInd(int x, int y)
    {
        int res = x - y;
        if (res < 0)
            res += _bufsize;
        assert(res>=0 && res<_bufsize);
        return res;
    }

    int _sizeof_interface;

    char* _base;

    int* _writeind;

    int* _readind;

    int* _done1;

    int* _opened;

    int _count;

private:

    std::string _fname;

    int _bufsize;

    CommMode _mode;

};

class SinkInterface : public Interface
{
public:
    SinkInterface(spTranState state, std::string fname, int sizeofitem, CommMode mode);

    ~SinkInterface() override;

    void* GetWritepointer()
    {
        return &_base[(*_writeind) * _sizeof_interface];
    }
    void Produce(int n)
    {
        *_writeind = AddInd(*_writeind, n);
    }

    spTranState GetTranstate(){ return _transtate; }

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output) override;

    spTranState _transtate;
};


class SourceInterface : public Interface
{
public:
    SourceInterface(spTranState state, std::string fname, int sizeofitem, CommMode mode);

    ~SourceInterface() override;

    void* GetReadpointer()
    {
        return &_base[(*_readind) * _sizeof_interface];
    }

    void Consume(int n)
    {
        *_readind = AddInd(*_readind, n);
    }

    spTranState GetTranstate(){ return _transtate; }

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, int& ninput, std::vector<const void*>& input, std::vector<void*>& output) override;

    spTranState _transtate;
};




class MakeInterface
{
public:

    typedef std::shared_ptr<SinkInterface> spSink;
    typedef std::shared_ptr<SourceInterface> spSource;
    typedef std::shared_ptr<TranState> spTranstate;
    typedef Interface::CommMode CommMode;



    MakeInterface(CommMode mode, const std::string& fname, const std::string& myname, const std::string& partnername, int sizeofitem)
        : _sinkapi(nullptr), _sourceapi(nullptr), _mode(mode)
    {
        if (mode == Interface::Tx)
        {
            _state = std::make_shared<TranState>(TranState::SEND);
            std::string fname_tx = fname + "_" + myname + "to" + partnername;
            _sinkapi = std::make_shared<SinkInterface>(_state,
                                                       fname_tx,
                                                       sizeofitem,
                                                       mode);
        }
        else if (mode == Interface::Rx)
        {
            _state = std::make_shared<TranState>(TranState::RECEIVE);
            std::string fname_rx = fname + "_" + partnername + "to" + myname;
            _sourceapi = std::make_shared<SourceInterface>(_state,
                                                           fname_rx,
                                                           sizeofitem,
                                                           mode);
        }
        else if (mode == Interface::TxRx_HalfD)
        {
            _state = std::make_shared<TranState>(TranState::RECEIVE);
            std::string fname_tx = fname + "_" + myname + "to" + partnername;
            std::string fname_rx = fname + "_" + partnername + "to" + myname;
            _sinkapi = std::make_shared<SinkInterface>(_state,
                                                       fname_tx,
                                                       sizeofitem,
                                                       mode);
            _sourceapi = std::make_shared<SourceInterface>(_state,
                                                           fname_rx,
                                                           sizeofitem,
                                                           mode);
        }
        else if (mode == Interface::TxRx_FullD)
        {
            _state = std::make_shared<TranState>(TranState::RECEIVE_SEND);
            std::string fname_tx = fname + "_" + myname + "to" + partnername;
            std::string fname_rx = fname + "_" + partnername + "to" + myname;
            _sinkapi = std::make_shared<SinkInterface>(_state,
                                                       fname_tx,
                                                       sizeofitem,
                                                       mode);
            _sourceapi = std::make_shared<SourceInterface>(_state,
                                                           fname_rx,
                                                           sizeofitem,
                                                           mode);
        }
        else
        {
            std::runtime_error("MakeInterface: Wrong Communication Mode! It should be Tx, Rx, TxRx_HalfD, or TxRx_FullD.");
        }
    }

    ~MakeInterface() {} ;

    spSink GetSink()
    {
        if (_mode == Interface::Rx)
        {
            std::runtime_error("MakeInterface: Rx mode, has no Sink API.");
        }
        return _sinkapi;
    }

    spSource GetSource()
    {
        if (_mode == Interface::Tx)
        {
            std::runtime_error("MakeInterface: Tx mode, has no Source API.");
        }
        return _sourceapi;
    }

private:
    spSink _sinkapi;

    spSource _sourceapi;

    spTranstate _state;

    CommMode _mode;

};

#endif //SIMPLATFORM_INTERFACE_H
