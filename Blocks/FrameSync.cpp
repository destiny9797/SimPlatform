//
// Created by zhujiaying on 2021/4/13.
//

#include "FrameSync.h"

FrameSync::FrameSync(std::string name, std::string header)
    : BasicBlock(name, 1, sizeof(uint8_t), 1, sizeof(uint8_t)),
      _header(header),
      _headerlen(header.size()),
      _nrshift(0)
{
    SetHistory(_headerlen);
}

FrameSync::~FrameSync() noexcept
{

}

void FrameSync::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput + GetHistory() - 1;
}

int FrameSync::work(int noutput, int& ninput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const uint8_t* in = (const uint8_t*)input[0];
    uint8_t* out = (uint8_t*)output[0];

    int j = 0;
    for (int i=0; i<noutput; ++i)
    {
        if (_nrshift > 0)
        {
            --_nrshift;
        }
        else
        {
            int correlation = 0;
            for (int j=0; j<GetHistory(); ++j)
            {
                correlation += _header[j]=='1'? in[i+j] : -in[i+j];
            }
            if (correlation >= _headerlen-1 || correlation <= 1-_headerlen){
                _nrshift = _headerlen-1;
            }
            else
            {
                out[j] = in[i];
                ++j;
            }
        }

    }

    ninput = noutput;
    return j;
}