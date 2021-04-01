//
// Created by zhujiaying on 2021/3/31.
//

#ifndef SIMPLATFORM_MSGCONTROL_H
#define SIMPLATFORM_MSGCONTROL_H

#include "BasicBlock.h"
#include "TranState.h"
#include "TaskPool.h"

class MsgControl : virtual public BasicBlock
{
public:
    typedef std::shared_ptr<TranState> spTranState;
    typedef std::shared_ptr<TaskPool> sptaskpool;

    typedef std::function<int(int, uint8_t*)> Task;

    void SetTaskpool(sptaskpool t){ _taskpool = t; }


protected:
    sptaskpool _taskpool;

};

class MsgGenerater : public MsgControl
{
public:
    MsgGenerater(spTranState state);

    ~MsgGenerater();

    spTranState GetTranstate(){ return _transtate; }

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, std::vector<const void*>& input, std::vector<void*>& output) override;

    spTranState _transtate;

};

class MsgParser : public MsgControl
{
public:
    MsgParser(spTranState state);

    ~MsgParser();

    spTranState GetTranstate(){ return _transtate; }

private:
    void forecast(int noutput, int& ninput_required) override;

    int work(int noutput, std::vector<const void*>& input, std::vector<void*>& output) override;

    spTranState _transtate;
};

#endif //SIMPLATFORM_MSGCONTROL_H
