//
// Created by zhujiaying on 2021/3/24.
//

#include "ConvertByteBit.h"

ConvertByteBit::ConvertByteBit(bool byte_to_bit)
    : BasicBlock(1,sizeof(uint8_t),1,sizeof(uint8_t)),
      _bytetobit(byte_to_bit)
{
    if (byte_to_bit)
        SetInterpolation(8);
    else
        SetDecimation(8);
}

ConvertByteBit::~ConvertByteBit()
{

}

void ConvertByteBit::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput / GetInterpolation() * GetDecimation() + GetHistory() - 1;
}

int ConvertByteBit::work(int noutput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const uint8_t* in = (const uint8_t*)input[0];
    uint8_t* out = (uint8_t*)output[0];

    if (_bytetobit)
    {
        assert(noutput % 8 == 0);
        for (int i=0; i<noutput/8; ++i)
        {
            uint8_t abyte = in[i];
            for (int j=8*i; j<8*i+8; ++j)
            {
                out[j] = (abyte & 0x80) >> 7;
                abyte <<= 1;
            }
        }
    }
    else
    {
        for (int i=0; i<noutput; ++i)
        {
            uint8_t abyte = 0;
            for (int j=8*i; j<8*i+8; ++j)
            {
                abyte <<= 1;
                abyte |= in[j];
            }
            out[i] = abyte;
        }
    }


    return noutput;
}