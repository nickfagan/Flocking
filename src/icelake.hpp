#ifndef ICE_LAKE_HPP
#define ICE_LAKE_HPP

#include "bird.hpp"

class IceLake {
public:
  IceLake();

  void initialize(bool mode_all);
  void render();

  void detect_collision(Bird *bird);

  double* get_clip_plane();
  void reflect();

private:
  bool m_mode_all;
};

#endif
