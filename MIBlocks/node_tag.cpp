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
#include "node_tag.h"

namespace gr {
  namespace MI_Communication {

    node_tag::sptr
    node_tag::make(bool isfixednode, bool random_location, int random_direction,
                   float distance,
                   float x, float y, float z,
                   float alpha, float beta, float gamma)
    {
      return gnuradio::get_initial_sptr
        (new node_tag_impl(isfixednode, random_location, random_direction, distance, x, y, z, alpha, beta, gamma));
    }

    /*
     * The private constructor
     */
    node_tag_impl::node_tag_impl(bool isfixednode, bool random_location, int random_direction,
                                 float distance,
                                 float x, float y, float z,
                                 float alpha, float beta, float gamma)
      : gr::block("node_tag",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              d_distance(distance),
              d_isfixednode(isfixednode),
              d_random_location(random_location),
              d_random_direction(random_direction)
    {
//        if (d_isfixednode){
            d_node.coord = Vect(0,0,0);
            d_node.direction = Rotate(0,0,0);
//        }
//        else {
            if (!d_random_location) {
                d_node.coord = Vect(x, y, z);
            }
            if (d_random_direction == 0){
                // set direction outside
                d_node.direction = Rotate(alpha, beta, gamma);
            }
//        }

        srand(time(0));
        if (!d_isfixednode){
            if (d_random_location){
                // randomly generate
                //d_node.coord = Vect(0,0,-5);
                bool x_positive = rand()%10 > 4;
                bool y_positive = rand()%10 > 4;
                bool z_positive = false;
                d_node.coord.x = (float)rand()/(float)RAND_MAX;
                d_node.coord.y = (float)rand()/(float)RAND_MAX;
                d_node.coord.z = (float)rand()/(float)RAND_MAX;
                d_node.coord.x = x_positive ? d_node.coord.x : -d_node.coord.x;
                d_node.coord.y = y_positive ? d_node.coord.y : -d_node.coord.y;
                d_node.coord.z = z_positive ? d_node.coord.z : -d_node.coord.z;

                // normalization
                float module = sqrt(d_node.coord.x*d_node.coord.x +
                                    d_node.coord.y*d_node.coord.y +
                                    d_node.coord.z*d_node.coord.z);
                d_node.coord.x = d_distance * d_node.coord.x / module;
                d_node.coord.y = d_distance * d_node.coord.y /module;
                d_node.coord.z = d_distance * d_node.coord.z / module;
            }
            if (d_random_direction == 1){
                // randomly generate
                d_node.direction.alpha = (float)rand()/(float)RAND_MAX * 2.0*PI;
                d_node.direction.beta = (float)rand()/(float)RAND_MAX * 2.0*PI;
                d_node.direction.gamma = (float)rand()/(float)RAND_MAX * 2.0*PI;
            }
            if (d_random_direction == 2) {
                // align towards the fixed node
                float x0 = 0 - d_node.coord.x;
                float y0 = 0 - d_node.coord.y;
                float z0 = 0 - d_node.coord.z;
                d_node.direction.alpha = (y0>0) ? acos(z0/sqrt(y0*y0+z0*z0)) : -acos(z0/sqrt(y0*y0+z0*z0));
                d_node.direction.beta = -asin(x0/sqrt(x0*x0+y0*y0+z0*z0));
                d_node.direction.gamma = 0;
            }
        }
    }

    /*
     * Our virtual destructor.
     */
    node_tag_impl::~node_tag_impl()
    {
    }


  } /* namespace MI_Communication */
} /* namespace gr */

