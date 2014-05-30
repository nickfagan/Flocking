#ifndef SKY_BOX_HPP
#define SKY_BOX_HPP

#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include "algebra.hpp"

class SkyBox {
public:
  SkyBox();

  void initialize(bool mode_all);
  void render();

private:
  GLuint load_skybox_png(std::string png_file);
  void draw_skybox(GLuint texture, float tex_coords[4][2], Point3D box_coords[4]);

private:
  bool m_mode_all;
  GLuint m_textures[6];
};

#endif
