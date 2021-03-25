//
// Created by zhujiaying on 12/14/20.
//

#ifndef GR_MI_COMMUNICATION_COORD_TRANSFER_H
#define GR_MI_COMMUNICATION_COORD_TRANSFER_H

#include "math.h"

void transfer(float alpha, float beta, float gamma,
              float old_x, float old_y, float old_z,
              float &new_x, float &new_y, float &new_z){
    new_x = old_x * (cos(beta)*cos(gamma)) +
            old_y * (cos(beta)*sin(gamma)) +//(-cos(alpha)*sin(gamma)+sin(alpha)*sin(beta)*cos(gamma)) +
            old_z * (-sin(beta));//(sin(alpha)*sin(gamma)+cos(alpha)*sin(beta)*cos(gamma));
    new_y = old_x * (-cos(alpha)*sin(gamma)+sin(alpha)*sin(beta)*cos(gamma)) + //(cos(beta)*sin(gamma)) +
            old_y * (cos(alpha)*cos(gamma)+sin(alpha)*sin(beta)*sin(gamma)) +
            old_z * (sin(alpha)*cos(beta)); //(-sin(alpha)*cos(gamma)+cos(alpha)*sin(beta)*sin(gamma));
    new_z = old_x * (sin(alpha)*sin(gamma)+cos(alpha)*sin(beta)*cos(gamma)) + //(-sin(beta)) +
            old_y * (-sin(alpha)*cos(gamma)+cos(alpha)*sin(beta)*sin(gamma)) + //(sin(alpha)*cos(beta)) +
            old_z * (cos(alpha)*cos(beta));
}
void retransfer(float alpha, float beta, float gamma,
                float old_x, float old_y, float old_z,
                float &new_x, float &new_y, float &new_z){
    new_x = old_x * (cos(gamma)*cos(beta)) +
            old_y * (cos(gamma)*sin(beta)*sin(alpha)-sin(gamma)*cos(alpha)) + //(sin(gamma)*cos(beta)) +
            old_z * (cos(gamma)*sin(beta)*cos(alpha)+sin(gamma)*sin(alpha));//(-sin(beta));
    new_y = old_x * (sin(gamma)*cos(beta)) + //(cos(gamma)*sin(beta)*sin(alpha)-sin(gamma)*cos(alpha)) +
            old_y * (sin(gamma)*sin(beta)*sin(alpha)+cos(gamma)*cos(alpha)) +
            old_z * (sin(gamma)*sin(beta)*cos(alpha)-cos(gamma)*sin(alpha));//(cos(beta)*sin(alpha));
    new_z = old_x * (-sin(beta)) + //(cos(gamma)*sin(beta)*cos(alpha)+sin(gamma)*sin(alpha)) +
            old_y * (cos(beta)*sin(alpha)) + //(sin(gamma)*sin(beta)*cos(alpha)-cos(gamma)*sin(alpha)) +
            old_z * (cos(beta)*cos(alpha));

}

#endif //GR_MI_COMMUNICATION_COORD_TRANSFER_H
