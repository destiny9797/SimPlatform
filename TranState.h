//
// Created by zhujiaying on 2021/3/31.
//

#ifndef SIMPLATFORM_ControlCenter_H
#define SIMPLATFORM_ControlCenter_H


class TranState
{
public:
    enum transceiver_state
    {
        RECEIVE,
        SEND,
        RECEIVE_SEND
    };

    TranState(transceiver_state state) : _transtate(state) {}

    ~TranState(){}

    void SetTranState(transceiver_state state){ _transtate = state; }

    transceiver_state GetTranState(){ return _transtate; }


private:
    transceiver_state _transtate;
};

#endif //SIMPLATFORM_ControlCenter_H
