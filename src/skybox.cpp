#include "skybox.hpp"
#include "image.hpp"
#include "algebra.hpp"
#include "constants.hpp"
#include <iostream>
#include <cstdio>

SkyBox::SkyBox() {

}

void SkyBox::initialize(bool mode_all) {
  m_mode_all = mode_all;

  if(!mode_all) return;

  m_textures[0] = load_skybox_png("data/front.png");
  m_textures[1] = load_skybox_png("data/back.png");
  m_textures[2] = load_skybox_png("data/left.png");
  m_textures[3] = load_skybox_png("data/right.png");
  m_textures[4] = load_skybox_png("data/top.png");
}

GLuint SkyBox::load_skybox_png(std::string png_file) {
  Image image;
  bool result = image.loadPng(png_file);

  float *data = new float[3*image.width()*image.height()];

  int x, y;
  for(y = 0; y < image.height(); y++) {
    for(x = 0; x < image.width(); x++) {
      data[3*(y*image.height() + x) + 0] = image(x, y, 0);
      data[3*(y*image.height() + x) + 1] = image(x, y, 1);
      data[3*(y*image.height() + x) + 2] = image(x, y, 2); 
    }
  }

  GLuint texture;
	
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_TEXTURE); 
  gluBuild2DMipmaps( GL_TEXTURE_2D, 3, image.width(), image.height(), GL_RGB, GL_FLOAT, data );
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);	

  delete [] data;

  return texture;
}

void SkyBox::render() {

  if(!m_mode_all) return;

  glPushMatrix();

  float tex_coords[][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

  Point3D p1 = Point3D(-WORLD_BOUNDS, -WORLD_BOUNDS, WORLD_BOUNDS);
  Point3D p2 = Point3D(-WORLD_BOUNDS, WORLD_BOUNDS, WORLD_BOUNDS);
  Point3D p3 = Point3D(WORLD_BOUNDS, WORLD_BOUNDS, WORLD_BOUNDS);
  Point3D p4 = Point3D(WORLD_BOUNDS, -WORLD_BOUNDS, WORLD_BOUNDS);
  Point3D p5 = Point3D(-WORLD_BOUNDS, -WORLD_BOUNDS, -WORLD_BOUNDS);
  Point3D p6 = Point3D(-WORLD_BOUNDS, WORLD_BOUNDS, -WORLD_BOUNDS);
  Point3D p7 = Point3D(WORLD_BOUNDS, WORLD_BOUNDS, -WORLD_BOUNDS);
  Point3D p8 = Point3D(WORLD_BOUNDS, -WORLD_BOUNDS, -WORLD_BOUNDS);

  Point3D box_coords[][4] = {{p6, p7, p8, p5},
			{p3, p2, p1, p4},
			{p2, p6, p5, p1},
			{p7, p3, p4, p8},
                        {p2, p3, p7, p6}};

  glEnable(GL_TEXTURE_2D);

  glScalef(10, 10, 10);

  int i;
  for(i = 0; i < 5; i++) {
    draw_skybox(m_textures[i], tex_coords, box_coords[i]);
  }

  glDisable(GL_TEXTURE_2D);

  glPopMatrix();
}

void SkyBox::draw_skybox(GLuint texture, float tex_coords[4][2], Point3D box_coords[4]) {
  GLfloat mat_diffuse[] = {1.0, 1.0, 1.0};
  GLfloat mat_specular[] = {0.0, 0.0, 0.0};
  GLfloat mat_shininess[] = { 50 };

  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  glBindTexture(GL_TEXTURE_2D, texture);

  glBegin(GL_QUADS);
  glTexCoord2f(tex_coords[0][0], tex_coords[0][1]); 
  glVertex3f(box_coords[0][0], box_coords[0][1], box_coords[0][2]);

  glTexCoord2f(tex_coords[1][0], tex_coords[1][1]); 
  glVertex3f(box_coords[1][0], box_coords[1][1], box_coords[1][2]);

  glTexCoord2f(tex_coords[2][0], tex_coords[2][1]); 
  glVertex3f(box_coords[2][0], box_coords[2][1], box_coords[2][2]);

  glTexCoord2f(tex_coords[3][0], tex_coords[3][1]); 
  glVertex3f(box_coords[3][0], box_coords[3][1], box_coords[3][2]);
  glEnd();
}

