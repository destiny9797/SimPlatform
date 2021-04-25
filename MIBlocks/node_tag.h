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

#ifndef INCLUDED_MI_COMMUNICATION_NODE_TAG_IMPL_H
#define INCLUDED_MI_COMMUNICATION_NODE_TAG_IMPL_H

#include <MI_Communication/node_tag.h>

namespace gr {
  namespace MI_Communication {

    class node_tag_impl : public node_tag {
    private:
        // Nothing to declare in this block.
        bool d_isfixednode;
        bool d_random_location;
        int d_random_direction;
        float d_distance;

    public:
        node_tag_impl(bool isfixednode, bool random_location, int random_direction,
                      float distance,
                      float x = 0.0, float y = 0.0, float z = -5.0,
                      float alpha = 0.0, float beta = 0.0, float gamma = 0.0);

        ~node_tag_impl();

        Node d_node;
        Node get_node(){ return this->d_node;}
    };
  } // namespace MI_Communication
} // namespace gr

#endif /* INCLUDED_MI_COMMUNICATION_NODE_TAG_IMPL_H */

