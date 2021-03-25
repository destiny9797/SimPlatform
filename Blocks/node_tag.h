
#ifndef INCLUDED_MI_COMMUNICATION_NODE_TAG_H
#define INCLUDED_MI_COMMUNICATION_NODE_TAG_H


#include "../BasicBlock.h"
#define PI 3.14159265358979323846

class node_tag
{
 public:

    node_tag(bool isfixednode, bool random_location, int random_direction,
                  float distance,
                  float x = 0.0, float y = 0.0, float z = -5.0,
                  float alpha = 0.0, float beta = 0.0, float gamma = 0.0);

    ~node_tag();

    struct Vect{
        float x;
        float y;
        float z;

        Vect():x(0.0), y(0.0), z(0.0){}
        Vect(const Vect &a):x(a.x), y(a.y), z(a.z){}
        Vect(float xx, float yy, float zz): x(xx), y(yy), z(zz){}
    };
    struct Rotate{
        float alpha;
        float beta;
        float gamma;

        Rotate():alpha(0.0), beta(0.0), gamma(0.0){}
        Rotate(const Rotate &a):alpha(a.alpha), beta(a.beta), gamma(a.gamma){}
        Rotate(float xx, float yy, float zz): alpha(xx), beta(yy), gamma(zz){}
    };
    struct Node{
        struct Vect coord;
        struct Rotate direction;

        Node():coord(), direction(){}
        Node(struct Vect _coord, struct Rotate _direction): coord(_coord), direction(_direction){}
    };

    Node d_node;
    Node get_node(){ return d_node;}
private:
    bool d_isfixednode;
    bool d_random_location;
    int d_random_direction;
    float d_distance;
};


#endif /* INCLUDED_MI_COMMUNICATION_NODE_TAG_H */

