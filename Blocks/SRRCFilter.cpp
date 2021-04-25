//
// Created by zhujiaying on 2021/3/24.
//

#include "SRRCFilter.h"
#include <complex>
#include <iostream>

SRRCFilter::SRRCFilter(std::string name, float gain, float rolloff, int ntaps, int interpolation)
    : BasicBlock(name, 1,sizeof(float),1,sizeof(float)),
      _gain(gain),
      _rolloff(rolloff),
      _ntaps(ntaps),
      _sps(interpolation),
      _taps(ntaps*interpolation)
{
    SetHistory(_ntaps);
    SetInterpolation(interpolation);
    makeTaps();
}


SRRCFilter::~SRRCFilter()
{

}


void SRRCFilter::makeTaps() {
    _ntaps = _ntaps % 2 ? _ntaps : _ntaps + 1; //ensure that ntaps is odd
    int ntaps = _ntaps * GetInterpolation();
    float spb = 1.0*_sps;    //samples per symbol
    std::vector<float> num(ntaps);
    std::vector<float> den(ntaps);
    float scale = 0.0;
    for (int i = 0; i < ntaps; i++) {
        int t = i - ntaps / 2;
        float x1 = PI * t / spb;
        float x2 = 4.0 * _rolloff * t / spb;
        float x3 = x2 * x2 - 1;
        if (x3 <= -0.000001 || x3 >= 0.000001) {         //Avoid Rounding errors...
            if (i != ntaps / 2)
                num[i] = cos((1.0 + _rolloff)*x1) + sin((1.0 - _rolloff)*x1) / (4.0 * _rolloff * t / spb);
            else
                num[i] = cos((1.0 + _rolloff)*x1) + (1.0 - _rolloff) * PI / (4.0 * _rolloff);
            den[i] = x3 * PI;
        }
        else {
            if (_rolloff == 1.0) {
                _taps[i] = -1.0;
                continue;
            }
            x3 = (1.0 - _rolloff)*x1;
            x2 = (1.0 + _rolloff)*x1;
            num[i] = sin(x2)*(1 + _rolloff)*PI - cos(x3)*((1.0 - _rolloff)*PI*spb) / (4.0 * _rolloff*t) + sin(x3)*spb*spb / (4.0 * _rolloff*t*t);
            den[i] = -32.0 * PI * _rolloff * _rolloff * t / spb;
        }
        _taps[i] = 4.0 * _rolloff * num[i] / den[i];
        scale += _taps[i];
    }
    for (int i = 0; i < ntaps; i++)
    {
        _taps[i] = _taps[i] * _gain / scale;
    }

}


void SRRCFilter::forecast(int noutput, int &ninput_required)
{
    ninput_required = noutput / GetInterpolation() + GetHistory() - 1;
}


int SRRCFilter::work(int noutput, int& ninput, std::vector<const void *> &input, std::vector<void *> &output)
{
    const float* in = (const float*)input[0];
    float* out = (float*)output[0];

    int inputnum = (noutput+(GetHistory()-1)*GetInterpolation()) / GetInterpolation();
    std::cout << "inputnum=" << inputnum << ", outputnum=" << noutput << std::endl;
    std::vector<float> in_itpl(inputnum*GetInterpolation(),0);
    int offset = 0;
    int j = 0;
    for (int i=0; i<inputnum; ++i)
    {
        offset = 0;
        in_itpl[j] = in[i];
        ++offset;
        while (offset < _sps)
        {
            in_itpl[j+offset] = 0;
            ++offset;
        }
        j += offset;
    }

    for (int i=0; i<noutput; ++i){
        float asum = 0;
        for (int j=i; j<i+_ntaps*_sps; ++j)
        {
//            std::cout << in_itpl[j] << " ";
            asum += in_itpl[j]*_taps[j-i];
        }
        out[i] = asum;
//        std::cout << out[i] << " ";
//        std::cout << std::endl;
    }

    ninput = noutput / GetInterpolation();

    return noutput;
}

