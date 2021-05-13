#ifndef SIMPLATFORM_NODE_TAG_H
#define SIMPLATFORM_NODE_TAG_H

#include <MI_Communication/node_tag.h>

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

#endif /* SIMPLATFORM_NODE_TAG_H */

