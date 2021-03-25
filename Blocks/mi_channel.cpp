
#include "coord_transfer.h"
#include "mi_channel.h"
#include <math.h>
#include <iostream>


void idft(const std::vector<std::complex<float>> &input, std::vector<std::complex<float>> &output, unsigned N){
  for (int n=0; n<N/2; ++n){
      std::complex<float> sum(0.0, 0.0);
      for (int k=0; k<N; ++k){
          sum += input[k] * exp(std::complex<float>(0,2*PI*n*k/((float)N)));
      }
      std::complex<float> complexN((float)N, 0.0);
      output[n] = sum/complexN;
  }
}


mi_channel::mi_channel(int turns, float radius, float sigma, float freq, float sample_rate,
                             std::shared_ptr<node_tag> node1,
                             std::shared_ptr<node_tag> node2)
: BasicBlock(3,sizeof(std::complex<float>),3,sizeof(std::complex<float>)),
      d_turns(turns),
      d_radius(radius),
      d_sample_rate(sample_rate),
      d_sigma(sigma),
      R0(0.01),
      mu(4*PI*pow(10,-7)),
      d_w(2*PI*freq)
{
    d_node1 = node1->get_node();
    d_node2 = node2->get_node();
    std::cout<<"Tx node location: " <<d_node1.coord.x << "," << d_node1.coord.y << "," << d_node1.coord.z << std::endl;
    std::cout<<"Rx node location: " <<d_node2.coord.x << "," << d_node2.coord.y << "," << d_node2.coord.z << std::endl;
    d_R = 2.0*PI*radius*turns*R0;
    d_L = mu*PI*pow(turns,2)*radius/2.0;
    d_C = 1.0/(d_w*d_w*d_L);

    init();
}


mi_channel::~mi_channel()
{
}

void
mi_channel::init() {
    d_distance = sqrt((d_node1.coord.x-d_node2.coord.x)*(d_node1.coord.x-d_node2.coord.x)
                      + (d_node1.coord.y-d_node2.coord.y)*(d_node1.coord.y-d_node2.coord.y)
                      + (d_node1.coord.z-d_node2.coord.z)*(d_node1.coord.z-d_node2.coord.z));
    // calculate
//        std::cout << "node1: " << d_node1.coord.x << " " << d_node1.coord.y << " " << d_node1.coord.z << std::endl;
//        std::cout << "node2: " << d_node2.coord.x << " " << d_node2.coord.y << " " << d_node2.coord.z << std::endl;
//        std::cout << "distance " << d_distance << std::endl;
    set_T();
    set_R();
    set_M();
    set_H();
    d_h.resize(9,std::vector<std::complex<float>>(d_nfft,0));
    dp_h.resize(9,0);
    for (int i=1; i<=3; i++)
        for (int j=1; j<=3; j++){
            idft(d_H[ind(i,j)], d_h[ind(i,j)], 2*d_nfft);
            dp_h[ind(i,j)] = (std::complex<float>*)malloc(d_nfft*sizeof(std::complex<float>));
        }
    for (int i=1; i<=3; i++){
        for (int j=1; j<=3; j++){
            for (int kk=0; kk<d_nfft; kk++){
                dp_h[ind(i,j)][kk] = d_h[ind(i,j)][d_nfft-1-kk];
            }
        }
    }

//        std::cout << "d_H[0]" << d_H[0][0] << std::endl;
//        std::cout << "d_h[0]" << d_h[0][0] << std::endl;

    SetHistory(d_nfft);
}
void
mi_channel::set_H()
{
    d_nfft = ceil(d_sample_rate/400000.0) * 256; // sample points number in freq
    d_Z.resize(d_nfft, 0);
    d_H.resize(9,std::vector<std::complex<float>>(2*d_nfft,0));
//        float _alpha = sqrt(PI*d_w/2/PI*mu*d_sigma);
//        float _G = exp(-d_distance*_alpha);

    for (int i=0; i<d_nfft; ++i){
        float _freq = d_sample_rate/2.0/(float)d_nfft * (float)i;
        float _w = 2.0*PI*_freq;
        float _alpha = sqrt(PI*_freq*mu*d_sigma);
        float _G = exp(-d_distance*_alpha);
        d_Z[i].real(d_R);
        d_Z[i].imag(_w*d_L - 1.0/(_w*d_C));

        for (int xx=1; xx<=3; xx++){
            for (int yy=1; yy<=3; yy++){
                int curr_index = ind(xx, yy);
                std::complex<float> temp(0.0, -_w * _G * d_M[curr_index]);
                d_H[curr_index][i] = d_R * temp/((d_Z[i]+d_R)*(d_Z[i]+d_R) + _w*_w*_G*d_M[curr_index]*_G*d_M[curr_index]);
            }
        }
    }
    for (int xx=1; xx<=3; xx++) {
        for (int yy = 1; yy <= 3; yy++) {
            int curr_index = ind(xx, yy);
            d_H[curr_index][d_nfft] = d_H[curr_index][d_nfft-1];
            for (int i=1; i<d_nfft ;++i){
                d_H[curr_index][i+d_nfft].real(d_H[curr_index][d_nfft-i].real());
                d_H[curr_index][i+d_nfft].imag(-d_H[curr_index][d_nfft-i].imag());
            }
        }
    }
}
void
mi_channel::forecast (int noutput, int& ninput_required)
{
    ninput_required = noutput + GetHistory() - 1;
}

int
mi_channel::work (int noutput, std::vector<const void*>& input, std::vector<void*>& output)
{
    const std::complex<float> *in1 = (const std::complex<float> *) input[0];
    const std::complex<float> *in2 = (const std::complex<float> *) input[1];
    const std::complex<float> *in3 = (const std::complex<float> *) input[2];
    std::complex<float> *out1 = (std::complex<float> *) output[0];
    std::complex<float> *out2 = (std::complex<float> *) output[1];
    std::complex<float> *out3 = (std::complex<float> *) output[2];



    for (int i=0; i<noutput; ++i)
    {
        std::complex<float> sum1(0.0, 0.0);
        std::complex<float> sum2(0.0, 0.0);
        std::complex<float> sum3(0.0, 0.0);

        // use volk to speed up the calculaiton

          for (int j=i; j<i+GetHistory(); ++j){
              sum1 += in1[j] * dp_h[0][d_nfft-1-(j-i)] +
                      in2[j] * dp_h[3][d_nfft-1-(j-i)] +
                      in3[j] * dp_h[6][d_nfft-1-(j-i)];
              sum2 += in1[j] * dp_h[1][d_nfft-1-(j-i)] +
                      in2[j] * dp_h[4][d_nfft-1-(j-i)] +
                      in3[j] * dp_h[7][d_nfft-1-(j-i)];
              sum3 += in1[j] * dp_h[2][d_nfft-1-(j-i)] +
                      in2[j] * dp_h[5][d_nfft-1-(j-i)] +
                      in3[j] * dp_h[8][d_nfft-1-(j-i)];
          }
          out1[i] = sum1;
          out2[i] = sum2;
          out3[i] = sum3;

//        std::cout << out1[i] << " ";
    }

    return noutput;
}

int
mi_channel::ind(unsigned x, unsigned y){
    if (x==1 && y==1)
        return 0;
    if (x==1 && y==2)
        return 1;
    if (x==1 && y==3)
        return 2;
    if (x==2 && y==1)
        return 3;
    if (x==2 && y==2)
        return 4;
    if (x==2 && y==3)
        return 5;
    if (x==3 && y==1)
        return 6;
    if (x==3 && y==2)
        return 7;
    if (x==3 && y==3)
        return 8;
    return -1;
}
void
mi_channel::set_T() {
    // pingyi zuobiaozhou
    Vect vectAB(d_node2.coord.x-d_node1.coord.x, d_node2.coord.y-d_node1.coord.y, d_node2.coord.z-d_node1.coord.z);
    // B zai A zuobiaoxi de zuobiao
    float x,y,z;
    transfer(d_node1.direction.alpha, d_node1.direction.beta, d_node1.direction.gamma,
             vectAB.x, vectAB.y, vectAB.z, x, y, z);
    // dairu qiu T
    Vect tempT[3];
    tempT[0].x = 3*x*z;
    tempT[0].y = 3*y*z;
    tempT[0].z = 2*z*z-x*x-y*y;
    tempT[1].x = 3*x*y;
    tempT[1].y = 2*y*y-x*x-z*z;
    tempT[1].z = 3*y*z;
    tempT[2].x = 2*x*x-y*y-z*z;
    tempT[2].y = 3*x*y;
    tempT[2].z = 3*x*z;
    // qiu T zai yuanzuobiaoxi de zuobiao
    retransfer(d_node1.direction.alpha, d_node1.direction.beta, d_node1.direction.gamma,
               tempT[0].x, tempT[0].y, tempT[0].z, vectT[0].x, vectT[0].y, vectT[0].z);
    retransfer(d_node1.direction.alpha, d_node1.direction.beta, d_node1.direction.gamma,
               tempT[1].x, tempT[1].y, tempT[1].z, vectT[1].x, vectT[1].y, vectT[1].z);
    retransfer(d_node1.direction.alpha, d_node1.direction.beta, d_node1.direction.gamma,
               tempT[2].x, tempT[2].y, tempT[2].z, vectT[2].x, vectT[2].y, vectT[2].z);
}
void
mi_channel::set_R(){
    retransfer(d_node2.direction.alpha, d_node2.direction.beta, d_node2.direction.gamma,
               0, 0, 1, vectR[0].x, vectR[0].y, vectR[0].z);
    retransfer(d_node2.direction.alpha, d_node2.direction.beta, d_node2.direction.gamma,
               0, 1, 0, vectR[1].x, vectR[1].y, vectR[1].z);
    retransfer(d_node2.direction.alpha, d_node2.direction.beta, d_node2.direction.gamma,
               1, 0, 0, vectR[2].x, vectR[2].y, vectR[2].z);
}
void
mi_channel::set_M(){
    float xishu = mu*PI*d_turns*d_turns*pow(d_radius,4)/(4*sqrt(pow(d_radius*d_radius+d_distance*d_distance,3)) * d_distance*d_distance);
    d_M.resize(9,0);
    for (int i=1; i<=3; i++){
        for (int j=1; j<=3; j++){
            d_M[ind(i,j)] = xishu * innerproduct(vectT[i-1], vectR[j-1]);
        }
    }
}
float
mi_channel::innerproduct(Vect A, Vect B) {
    return A.x*B.x+A.y*B.y+A.z*B.z;
}

