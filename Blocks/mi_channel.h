
#ifndef INCLUDED_MI_COMMUNICATION_MI_CHANNEL_H
#define INCLUDED_MI_COMMUNICATION_MI_CHANNEL_H

#include "../BasicBlock.h"
#include "node_tag.h"
#include <vector>
#include <complex>


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
  std::vector<std::complex<float>> d_Z;
  std::vector<float> d_M;
  std::vector<std::vector<std::complex<float>>> d_H;
  std::vector<std::vector<std::complex<float>>> d_h;
    std::vector<std::complex<float> *> dp_h;
  Vect vectT[3];
  Vect vectR[3];

  const float mu;
  const float R0;

    void set_H();
    void set_T();
    void set_R();
    void set_M();
    int ind(unsigned x, unsigned y);
    float innerproduct(Vect A, Vect B);

 public:

  mi_channel(int turns, float radius, float sigma, float freq, float sample_rate,
                  std::shared_ptr<node_tag> node1, std::shared_ptr<node_tag> node2);
  ~mi_channel() override;

  void forecast (int noutput, int& ninput_required) override;
  void init();

  int work(int noutput, std::vector<const void*>& input, std::vector<void*>& output) override;


};


#endif /* INCLUDED_MI_COMMUNICATION_MI_CHANNEL_H */

