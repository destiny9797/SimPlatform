/* -*- c++ -*- */
/* 
 * Copyright 2020 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/filter/fir_filter.h>
#include <math.h>
#include <volk/volk.h>
#include "MI_Communication/coord_transfer.h"
#include "mi_channel.h"



void idft(const std::vector<gr_complex> &input, std::vector<gr_complex> &output, unsigned N){
    for (int n=0; n<N/2; ++n){
        gr_complex sum(0.0, 0.0);
        for (int k=0; k<N; ++k){
          sum += input[k] * exp(gr_complex(0,2*PI*n*k/((float)N)));
        }
        gr_complex complexN((float)N, 0.0);
        output[n] = sum/complexN;
    }
}



mi_channel::mi_channel(int turns, float radius, float r0, float sigma, float freq, float sample_rate,
                             boost::shared_ptr<gr::MI_Communication::node_tag> node1,
                             boost::shared_ptr<gr::MI_Communication::node_tag> node2)
    : gr::block("mi_channel",
      gr::io_signature::make(1, 3, sizeof(gr_complex)),
      gr::io_signature::make(1, 3, sizeof(gr_complex))),
      d_turns(turns),
      d_radius(radius),
      d_sample_rate(sample_rate),
      d_sigma(sigma),
      R0(r0),
      mu(4*PI*pow(10,-7)),
      d_w(2*PI*freq)
{
    d_node1 = node1->get_node();
    d_node2 = node2->get_node();
    //        std::cout<<"Tx node location: " <<d_node1.coord.x << "," << d_node1.coord.y << "," << d_node1.coord.z << std::endl;
    //        std::cout<<"Rx node location: " <<d_node2.coord.x << "," << d_node2.coord.y << "," << d_node2.coord.z << std::endl;
    d_R = 2.0*PI*radius*turns*R0;
    d_L = mu*PI*pow(turns,2)*radius/2.0;
    d_C = 1.0/(d_w*d_w*d_L);

    init();
}

/*
* Our virtual destructor.
*/
mi_channel::~mi_channel()
{
}

void
mi_channel::init()
{
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
    d_h.resize(9,std::vector<gr_complex>(d_nfft,0));
    dp_h.resize(9,0);
    for (int i=1; i<=3; i++)
        for (int j=1; j<=3; j++)
        {
            idft(d_H[ind(i,j)], d_h[ind(i,j)], 2*d_nfft);
            dp_h[ind(i,j)] = (gr_complex*)malloc(d_nfft*sizeof(gr_complex));
        }
    for (int i=1; i<=3; i++)
    {
        for (int j=1; j<=3; j++)
        {
            for (int kk=0; kk<d_nfft; kk++)
            {
                dp_h[ind(i,j)][kk] = d_h[ind(i,j)][d_nfft-1-kk];
            }
        }
    }

    //        std::cout << "d_H[0]" << d_H[0][0] << std::endl;
    //        std::cout << "d_h[0]" << d_h[0][0] << std::endl;

    set_history(d_nfft);
}
void
mi_channel::set_H()
{
    d_nfft = ceil(d_sample_rate/400000.0) * 256; // sample points number in freq
    d_Z.resize(d_nfft, 0);
    d_H.resize(9,std::vector<gr_complex>(2*d_nfft,0));
    //        float _alpha = sqrt(PI*d_w/2/PI*mu*d_sigma);
    //        float _G = exp(-d_distance*_alpha);

    for (int i=0; i<d_nfft; ++i)
    {
        float _freq = d_sample_rate/2.0/(float)d_nfft * (float)i;
        float _w = 2.0*PI*_freq;
        float _alpha = sqrt(PI*_freq*mu*d_sigma);
        float _G = exp(-d_distance*_alpha);
        d_Z[i].real(d_R);
        d_Z[i].imag(_w*d_L - 1.0/(_w*d_C));

        for (int xx=1; xx<=3; xx++)
        {
            for (int yy=1; yy<=3; yy++)
            {
                int curr_index = ind(xx, yy);
                gr_complex temp(0.0, -_w * _G * d_M[curr_index]);
                d_H[curr_index][i] = d_R * temp/((d_Z[i]+d_R)*(d_Z[i]+d_R) + _w*_w*_G*d_M[curr_index]*_G*d_M[curr_index]);
            }
        }
    }
    for (int xx=1; xx<=3; xx++)
    {
        for (int yy = 1; yy <= 3; yy++)
        {
            int curr_index = ind(xx, yy);
            d_H[curr_index][d_nfft] = d_H[curr_index][d_nfft-1];
            for (int i=1; i<d_nfft ;++i)
            {
                d_H[curr_index][i+d_nfft].real(d_H[curr_index][d_nfft-i].real());
                d_H[curr_index][i+d_nfft].imag(-d_H[curr_index][d_nfft-i].imag());
            }
        }
    }
}
void
mi_channel::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
    /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    int nport = ninput_items_required.size();
    for (int i=0; i<nport; i++){
      ninput_items_required[i] = noutput_items;
    }
}

int
mi_channel::general_work (int noutput_items,
               gr_vector_int &ninput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items)
{
    const gr_complex *in1 = (const gr_complex *) input_items[0];
    const gr_complex *in2 = (const gr_complex *) input_items[1];
    const gr_complex *in3 = (const gr_complex *) input_items[2];
    gr_complex *out1 = (gr_complex *) output_items[0];
    gr_complex *out2 = (gr_complex *) output_items[1];
    gr_complex *out3 = (gr_complex *) output_items[2];

    // Do <+signal processing+>

    for (int i=0; i<noutput_items; ++i){
        gr_complex sum1(0.0, 0.0);
        gr_complex sum2(0.0, 0.0);
        gr_complex sum3(0.0, 0.0);

        // use volk to speed up the calculaiton
        unsigned int NN = history();
        lv_32fc_t tmp11,tmp21,tmp31,tmp12,tmp22,tmp32,tmp13,tmp23,tmp33;
        volk_32fc_x2_dot_prod_32fc(&tmp11,in1+i,dp_h[0],NN);
        volk_32fc_x2_dot_prod_32fc(&tmp21,in2+i,dp_h[3],NN);
        volk_32fc_x2_dot_prod_32fc(&tmp31,in3+i,dp_h[6],NN);
        volk_32fc_x2_dot_prod_32fc(&tmp12,in1+i,dp_h[1],NN);
        volk_32fc_x2_dot_prod_32fc(&tmp22,in2+i,dp_h[4],NN);
        volk_32fc_x2_dot_prod_32fc(&tmp32,in3+i,dp_h[7],NN);
        volk_32fc_x2_dot_prod_32fc(&tmp13,in1+i,dp_h[2],NN);
        volk_32fc_x2_dot_prod_32fc(&tmp23,in2+i,dp_h[5],NN);
        volk_32fc_x2_dot_prod_32fc(&tmp33,in3+i,dp_h[8],NN);

        out1[i] = tmp11 + tmp21 + tmp31;
        out2[i] = tmp12 + tmp22 + tmp32;
        out3[i] = tmp13 + tmp23 + tmp33;


        //          for (int j=i; j<i+history(); ++j){
        //              sum1 += in1[j] * dp_h[0][d_nfft-1-(j-i)] +
        //                      in2[j] * dp_h[3][d_nfft-1-(j-i)] +
        //                      in3[j] * dp_h[6][d_nfft-1-(j-i)];
        //              sum2 += in1[j] * dp_h[1][d_nfft-1-(j-i)] +
        //                      in2[j] * dp_h[4][d_nfft-1-(j-i)] +
        //                      in3[j] * dp_h[7][d_nfft-1-(j-i)];
        //              sum3 += in1[j] * dp_h[2][d_nfft-1-(j-i)] +
        //                      in2[j] * dp_h[5][d_nfft-1-(j-i)] +
        //                      in3[j] * dp_h[8][d_nfft-1-(j-i)];
        //          }
        //          out1[i] = sum1;
        //          out2[i] = sum2;
        //          out3[i] = sum3;

        //std::cout << out1[i] << " ";
    }

    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each (noutput_items);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

unsigned
mi_channel::ind(unsigned x, unsigned y)
{
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
}
void
mi_channel::set_T()
{
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
    //        std::cout << "T: alpha=" << d_node1.direction.alpha << " beta=" << d_node1.direction.beta << " gamma=" << d_node1.direction.gamma << std::endl;
    //        std::cout << "T:[" << vectT[0].x << "," << vectT[0].y << "," << vectT[0].z << ";"
    //                  << vectT[1].x << "," << vectT[1].y << "," << vectT[1].z << ";"
    //                  << vectT[2].x << "," << vectT[2].y << "," << vectT[2].z << "]" << std::endl;
}
void
mi_channel::set_R()
{
    retransfer(d_node2.direction.alpha, d_node2.direction.beta, d_node2.direction.gamma,
             0, 0, 1, vectR[0].x, vectR[0].y, vectR[0].z);
    retransfer(d_node2.direction.alpha, d_node2.direction.beta, d_node2.direction.gamma,
               0, 1, 0, vectR[1].x, vectR[1].y, vectR[1].z);
    retransfer(d_node2.direction.alpha, d_node2.direction.beta, d_node2.direction.gamma,
               1, 0, 0, vectR[2].x, vectR[2].y, vectR[2].z);

    //        std::cout << "R: alpha=" << d_node2.direction.alpha << " beta=" << d_node2.direction.beta << " gamma=" << d_node2.direction.gamma << std::endl;
    //        std::cout << "R:[" << vectR[0].x << "," << vectR[0].y << "," << vectR[0].z << ";"
    //            << vectR[1].x << "," << vectR[1].y << "," << vectR[1].z << ";"
    //            << vectR[2].x << "," << vectR[2].y << "," << vectR[2].z << "]" << std::endl;
}
void
mi_channel::set_M()
{
    float xishu = mu*PI*d_turns*d_turns*pow(d_radius,4)/(4*sqrt(pow(d_radius*d_radius+d_distance*d_distance,3)) * d_distance*d_distance);
    d_M.resize(9,0);
    for (int i=1; i<=3; i++){
        for (int j=1; j<=3; j++){
            d_M[ind(i,j)] = xishu * innerproduct(vectT[i-1], vectR[j-1]);
        }
    }
}
float
mi_channel::innerproduct(Vect A, Vect B)
{
    return A.x*B.x+A.y*B.y+A.z*B.z;
}


