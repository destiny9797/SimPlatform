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

#ifndef INCLUDED_MI_COMMUNICATION_MI_CHANNEL_IMPL_H
#define INCLUDED_MI_COMMUNICATION_MI_CHANNEL_IMPL_H

#include <../BasicBlock.h>
//#include <MI_Communication/node_generator.h>
#include <vector>


class mi_channel : public BasicBlock
{
private:
    typedef node_tag::Vect Vect;
    typedef node_tag::Node Node;
    Node d_node1, d_node2;

    int d_turns;
    float d_sample_rate;
    float d_sigma;
    float d_distance;
    float d_radius;


    float d_w;
    float d_R;
    float d_L;
    float d_C;
    unsigned d_nfft;
    std::vector<gr_complex> d_Z;
    std::vector<float> d_M;
    std::vector<std::vector<gr_complex>> d_H;
    std::vector<std::vector<gr_complex>> d_h;
    std::vector<gr_complex *> dp_h;
    Vect vectT[3];
    Vect vectR[3];

    const float mu;
    const float R0;

    void set_H();
    void set_T();
    void set_R();
    void set_M();
    unsigned ind(unsigned x, unsigned y);
    float innerproduct(Vect A, Vect B);

    public:
    mi_channel_impl(int turns, float radius, float r0, float sigma, float freq, float sample_rate,
                  boost::shared_ptr<gr::MI_Communication::node_tag> node1, boost::shared_ptr<gr::MI_Communication::node_tag> node2);
    ~mi_channel_impl();

    // Where all the action really happens
    void forecast (int noutput_items, gr_vector_int &ninput_items_required);
    void init();

    int general_work(int noutput_items,
       gr_vector_int &ninput_items,
       gr_vector_const_void_star &input_items,
       gr_vector_void_star &output_items);


};


#endif /* INCLUDED_MI_COMMUNICATION_MI_CHANNEL_IMPL_H */

