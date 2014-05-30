#include "icelake.hpp"
#include "algebra.hpp"
#include "constants.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

IceLake::IceLake() {

}

void IceLake::initialize(bool mode_all) {
  m_mode_all = mode_all;
}

void IceLake::render() {
  if(!m_mode_all) return;

  GLfloat mat_diffuse[] = {0.67, 0.97, 1.0, 0.60};
  GLfloat mat_specular[] = {1.0, 1.0, 1.0};
  GLfloat mat_shininess[] = { 25 };

  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  glPushMatrix();

  glTranslatef(0, -WORLD_BOUNDS+5, 0);

  glEnable(GL_BLEND);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glVertex3f(-WORLD_BOUNDS/1.5, 0, -WORLD_BOUNDS/1.5);
  glVertex3f(-WORLD_BOUNDS/1.5, 0, WORLD_BOUNDS/1.5);
  glVertex3f(WORLD_BOUNDS/1.5, 0, WORLD_BOUNDS/1.5);
  glVertex3f(WORLD_BOUNDS/1.5, 0, -WORLD_BOUNDS/1.5);
  glEnd();

  glDisable(GL_BLEND);

  glPopMatrix();
}

void IceLake::detect_collision(Bird *bird) {
  if(!m_mode_all) return;

  Vector3D normal(0, 1, 0);
  Point3D p1 = Point3D(-WORLD_BOUNDS/1.5, -WORLD_BOUNDS+5, -WORLD_BOUNDS/1.5);
  Point3D p2 = Point3D(-WORLD_BOUNDS/1.5, -WORLD_BOUNDS+5, WORLD_BOUNDS/1.5);
  Point3D p3 = Point3D(WORLD_BOUNDS/1.5, -WORLD_BOUNDS+5, WORLD_BOUNDS/1.5);
  Point3D p4 = Point3D(WORLD_BOUNDS/1.5, -WORLD_BOUNDS+5, -WORLD_BOUNDS/1.5);

  double facing = normal.dot(bird->get_velocity());

  // If the bird is not heading in the direction of the plane the mountain surface
  // lines on then we do not need to consider it.
  if(facing >= 0) return;

  double t = -normal.dot(bird->get_position() - p1) / facing;
  Point3D i_point = bird->get_position() + t * bird->get_velocity();

  Vector3D to_point = bird->get_position() - i_point;
  double distance = to_point.magnitude();

  if(distance <= COLLISION_DISTANCE) {
    bird->add_velocity((1.0/distance)*normal);
  }
}

double* IceLake::get_clip_plane() {
  Point3D p1 = Point3D(0, -WORLD_BOUNDS+5, 0);
  Point3D p2 = Point3D(1, -WORLD_BOUNDS+5, 0);
  Point3D p3 = Point3D(0, -WORLD_BOUNDS+5, 1);

  double* eq = new double[4];
  eq[0] = (p1[1]*(p2[2] - p3[2])) + (p2[1]*(p3[2] - p1[2])) + (p3[1]*(p1[2] - p2[2]));
  eq[1] = (p1[2]*(p2[0] - p3[0])) + (p2[2]*(p3[0] - p1[0])) + (p3[2]*(p1[0] - p2[0]));
  eq[2] = (p1[0]*(p2[1] - p3[1])) + (p2[0]*(p3[1] - p1[1])) + (p3[0]*(p1[1] - p2[1]));
  eq[3] = -((p1[0]*((p2[1]*p3[2]) - (p3[1]*p2[2]))) + (p2[0]*((p3[1]*p1[2]) - (p1[1]*p3[2]))) + (p3[0]*((p1[1]*p2[2]) - (p2[1]*p1[2]))));

  return eq;
}

void IceLake::reflect() {
  glTranslatef(0, -WORLD_BOUNDS+5, 0);
  glScalef(1, -1, 1);
  glTranslatef(0, WORLD_BOUNDS-5, 0);
}









