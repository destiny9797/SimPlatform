//
// Created by zhujiaying on 2021/3/29.
//

#ifndef SIMPLATFORM_INTERFACE_H
#define SIMPLATFORM_INTERFACE_H

#include "BasicBlock.h"
#include <string>

class Interface : public BasicBlock
{
public:
    char* GetBase()
    {
        return _base;
    }
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

protected:
    Interface(std::string fname, int sizeofitem);

    ~Interface();

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

    void SetSink()
    {
        _issinkinterface = true;
    }

    void SetSource()
    {
        _issourceinterfacce = true;
    }

    int _sizeof_interface;

    char* _base;

    int* _writeind;

    int* _readind;

    int* _done1;

private:

    std::string _fname;

    int _bufsize;

};

class SinkInterface : public Interface
{
public:
    enum noise_model{
        AWGN
    };
    enum fade_model{
        Rayleigh
    };
    SinkInterface(std::string fname, int sizeofitem, noise_model noisemodel, fade_model fademodel);

    ~SinkInterface() override;

    void* GetWritepointer()
    {
        return &_base[(*_writeind) * _sizeof_interface];
    }
    void Produce(int n)
    {
        *_writeind = AddInd(*_writeind, n);
    }

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, std::vector<const void*>& input, std::vector<void*>& output) override;
};


class SourceInterface : public Interface
{
public:
    SourceInterface(std::string fname, int sizeofitem);

    ~SourceInterface() override;

    void* GetReadpointer()
    {
        return &_base[(*_readind) * _sizeof_interface];
    }

    void Consume(int n)
    {
        *_readind = AddInd(*_readind, n);
    }

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, std::vector<const void*>& input, std::vector<void*>& output) override;
};

#endif //SIMPLATFORM_INTERFACE_H
