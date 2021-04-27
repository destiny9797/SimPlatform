//
// Created by zhujiaying on 2021/4/13.
//

#include "FrameSync.h"
#include <iostream>

FrameSync::FrameSync(std::string name, std::string header)
    : BasicBlock(name, 1, sizeof(uint8_t), 1, sizeof(uint8_t)),
      _header(header),
      _headerlen(header.size()),
      _nrshift(0),
      _findheader(false)
{
//    SetHistory(_headerlen);
}

FrameSync::~FrameSync() noexcept
{

}

void FrameSync::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput + _headerlen - 1;
}

int FrameSync::work(int noutput, int& ninput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const uint8_t* in = (const uint8_t*)input[0];
    uint8_t* out = (uint8_t*)output[0];

    int i = 0; //实际读入的个数
    int k = 0; //实际产生的个数
    int inmap;
    while (i<noutput && k<noutput)
    {
        int correlation = 0;
        for (int j=0; j<_headerlen; ++j)
        {
            assert((i+j)<noutput+_headerlen);
            inmap = in[i+j]==0 ? -1 : 1;
            correlation += _header[j]=='1'? inmap : -inmap;
        }
        if (correlation>=_headerlen-1 || correlation<=1-_headerlen)
        {
            _findheader = true;
            i += _headerlen;
        }
        else
        {
            if (_findheader)
            {
                out[k] = in[i];
                ++k;
            }
            ++i;
        }
    }
//    for (int i=0; i<noutput && k<noutput; ++i)
//    {
//        if (_nrshift > 0)
//        {
//            --_nrshift;
//        }
//        else
//        {
//            int correlation = 0;
//            for (int j=0; j<GetHistory() && i<noutput; ++j)
//            {
//                assert((i+j)<noutput+GetHistory()-1);
//                inmap = in[i+j]==0 ? -1 : 1;
//                correlation += _header[j]=='1'? inmap : -inmap;
//            }
//            if (correlation >= _headerlen-2 || correlation <= 2-_headerlen){
//                _nrshift = _headerlen-1;
//                _findheader = true;
//            }
//            else if (_findheader)
//            {
//                assert(k<noutput);
//                out[k] = in[i];
//                ++k;
//            }
//        }
//
//    }


    ninput = i;
    return k;
}