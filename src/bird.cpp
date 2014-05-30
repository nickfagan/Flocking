#include <iostream>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>

#include "algebra.hpp"
#include "bird.hpp"
#include "constants.hpp"
#include <GL/glut.h>

#define PI 3.14159;

Bird::Bird() {
}

Vector3D calc_normal(Point3D p1, Point3D p2, Point3D p3) {
  Vector3D n, u, v;

  u = p1 - p2;
  v = p1 - p3;
  n = u.cross(v);
  n.normalize();

  return n;
}

void Bird::initialize(bool mode_all, int id, Point3D position, Vector3D velocity, double local_dist, BinLattice bin_lattice) {
  m_mode_all = mode_all;
  m_id = id;
  m_position = position;
  m_velocity = velocity;
  m_local_dist = local_dist;
  m_bin_lattice = bin_lattice;
  m_binlattice_on = true;

  m_wing_angle = rand() % 91 - 45;
  m_wa_inc = 20;

  m_bin = NULL;
  set_bins();

  t_points[0] = Point3D(-0.5, -0.5, -0.75);
  t_points[1] = Point3D(-0.5, 0.5, -0.75);
  t_points[2] = Point3D(0.5, 0.5, -0.75);
  t_points[3] = Point3D(0.5, -0.5, -0.75);
  t_points[4] = Point3D(-0.0, 0.0, 0.75);

  t_normals[0] = calc_normal(t_points[1], t_points[0], t_points[1]);
  t_normals[1] = calc_normal(t_points[4], t_points[0], t_points[1]);
  t_normals[2] = calc_normal(t_points[4], t_points[3], t_points[0]);
  t_normals[3] = calc_normal(t_points[4], t_points[3], t_points[2]);
  t_normals[4] = calc_normal(t_points[4], t_points[2], t_points[1]);
}

void Bird::turn_binlattice_on(bool on) {
  m_binlattice_on = on;

  if(on) {
    m_bin = NULL;
    set_bins();
  }
}

Point3D Bird::get_position() {
  return m_position;
}

Vector3D Bird::get_velocity() {
  return m_velocity;
}

void Bird::add_velocity(Vector3D velocity) {
  m_velocity = m_velocity + velocity;
  m_velocity = m_velocity.limit(get_max_speed());

  Vector3D v = m_velocity;
  v.normalize();

  double dx = v[0];
  double dy = v[1];
  double dz = v[2];

  double hyp = sqrt(dx*dx + dz*dz);
  double x_angle = atan2(-dy, hyp);
  double y_angle = atan2(dx, dz);

  m_direction.x_rotation = x_angle*180/PI;
  m_direction.y_rotation = y_angle*180/PI;
}

void Bird::set_bins() {
  std::list<Bin*> bins;

  // If m_bin bin is null then this is the first time we are getting a bin
  if(m_bin == NULL) {
    m_bin = m_bin_lattice.get_bin(m_position[0], m_position[1], m_position[2]);
  }

  // If m_bin is still null that means the bird is outside all available bins which is bad, so we
  // set the birds position to the worlds origin and get the bin again.
  if(m_bin == NULL) {
    m_position = Point3D(0, 0, 0);
    m_bin = m_bin_lattice.get_bin(m_position[0], m_position[1], m_position[2]);
  }

  m_bins = m_bin_lattice.get_neighbour_bins(m_bin, m_local_dist);
}

void Bird::update() {
  m_position[0] += m_velocity[0];
  m_position[1] += m_velocity[1];
  m_position[2] += m_velocity[2];

  if(m_position[0] > WORLD_BOUNDS && m_velocity[0] > 0 || m_position[0] < -WORLD_BOUNDS && m_velocity[0] < 0) {
    m_velocity[0] = -m_velocity[0];
    m_position[0] += m_velocity[0];
  }

  if(m_position[1] > WORLD_BOUNDS && m_velocity[1] > 0 || m_position[1] < -WORLD_BOUNDS && m_velocity[1] < 0) {
    m_velocity[1] = -m_velocity[1];
  m_position[1] += m_velocity[1];
  }

  if(m_position[2] > WORLD_BOUNDS && m_velocity[2] > 0 || m_position[2] < -WORLD_BOUNDS && m_velocity[2] < 0) {
    m_velocity[2] = -m_velocity[2];
    m_position[2] += m_velocity[2];
  }

  if(m_binlattice_on) {
    Bin* new_bin = m_bin_lattice.get_new_bin(m_position, m_bin);
    if(new_bin != NULL) {
      m_bin->remove_bird(this, get_bird_type());
      m_bin = new_bin;
      m_bin->add_bird(this, get_bird_type());

      set_bins();
    }
  }

  m_wing_angle += m_wa_inc;

  if(m_wa_inc > 0 && m_wing_angle >= 40 || m_wa_inc < 0 && m_wing_angle <= -40) {
    m_wa_inc = -m_wa_inc;
  }
}

void Bird::render() {
  glPushMatrix();

  set_bird_material();

  glTranslatef(m_position[0], m_position[1], m_position[2]);
  glRotatef(m_direction.y_rotation, 0, 1, 0);
  glRotatef(m_direction.x_rotation, 1, 0, 0);

  if(m_mode_all) {
    draw_bird();
  } else {
    draw_basic_bird();
  }

  glPopMatrix();

  set_targets_local(false);
}

void Bird::set_bird_material() {
  double r, g, b;

  if(get_bird_type() == 0) {
    r = 0.7;
    g = 0.7;
    b = 0.7;
  } else if(get_bird_type() == 1) {
    r = 1;
    g = 0;
    b = 0;
  }

  GLfloat mat_diffuse[] = {r, g, b};
  GLfloat mat_specular[] = {0.2, 0.2, 0.2};
  GLfloat mat_shininess[] = { 10 };

  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void Bird::draw_bird() {
    glPushMatrix();
    glScalef(0.5, 0.5, 0.5);

    glPushMatrix();
    // Draw body
    glScalef(1, 1, 3);
    if(dl_index1 == 0) {
      dl_index1 = glGenLists(1);
      glNewList(dl_index1, GL_COMPILE);
      glutSolidSphere(1, 32, 32);
      glEndList();
    } else {
      glCallList(dl_index1);
    }
    glPopMatrix();

    glPushMatrix();
    glRotatef(-m_wing_angle, 0, 0, 1);
    // Draw wing1
    glTranslatef(-2.2, 0, 0);
    glScalef(2, 0.5, 2);
    if(dl_index2 == 0) {
      dl_index2 = glGenLists(1);
      glNewList(dl_index2, GL_COMPILE);
      glutSolidSphere(1, 32, 32);
      glEndList();
    } else {
      glCallList(dl_index2);
    }
    glPopMatrix();


    glPushMatrix();
    glRotatef(m_wing_angle, 0, 0, 1);
    // Draw wing1
    glTranslatef(2.2, 0, 0);
    glScalef(2, 0.5, 2);
    if(dl_index3 == 0) {
      dl_index3 = glGenLists(1);
      glNewList(dl_index3, GL_COMPILE);
      glutSolidSphere(1, 32, 32);
      glEndList();
    } else {
      glCallList(dl_index3);
    }
    glPopMatrix();

    glPopMatrix();
}

void Bird::draw_basic_bird() {
  glBegin(GL_QUADS);
  glNormal3f(t_normals[0][0], t_normals[0][1], t_normals[0][2]);
  glVertex3f(t_points[0][0], t_points[0][1], t_points[0][2]);
  glVertex3f(t_points[1][0], t_points[1][1], t_points[1][2]);
  glVertex3f(t_points[2][0], t_points[2][1], t_points[2][2]);
  glVertex3f(t_points[3][0], t_points[3][1], t_points[3][2]);
  glEnd();

  glBegin(GL_TRIANGLES);
  glNormal3f(t_normals[1][0], t_normals[1][1], t_normals[1][2]);
  glVertex3f(t_points[0][0], t_points[0][1], t_points[0][2]);
  glVertex3f(t_points[1][0], t_points[1][1], t_points[1][2]);
  glVertex3f(t_points[4][0], t_points[4][1], t_points[4][2]);
  glEnd();

  glBegin(GL_TRIANGLES);
  glNormal3f(t_normals[2][0], t_normals[2][1], t_normals[2][2]);
  glVertex3f(t_points[1][0], t_points[1][1], t_points[1][2]);
  glVertex3f(t_points[2][0], t_points[2][1], t_points[2][2]);
  glVertex3f(t_points[4][0], t_points[4][1], t_points[4][2]);
  glEnd();

  glBegin(GL_TRIANGLES);
  glNormal3f(t_normals[3][0], t_normals[3][1], t_normals[3][2]);
  glVertex3f(t_points[2][0], t_points[2][1], t_points[2][2]);
  glVertex3f(t_points[3][0], t_points[3][1], t_points[3][2]);
  glVertex3f(t_points[4][0], t_points[4][1], t_points[4][2]);
  glEnd();

  glBegin(GL_TRIANGLES);
  glNormal3f(t_normals[4][0], t_normals[4][1], t_normals[4][2]);
  glVertex3f(t_points[3][0], t_points[3][1], t_points[3][2]);
  glVertex3f(t_points[0][0], t_points[0][1], t_points[0][2]);
  glVertex3f(t_points[4][0], t_points[4][1], t_points[4][2]);
  glEnd();
}

/**************************************/
/*            PreyBird               */
/**************************************/

PreyBird::PreyBird() : Bird() {

}

int PreyBird::get_bird_type() {
  return 0;
}

double PreyBird::get_max_speed() {
  return PREY_MAX_SPEED;
}

/**************************************/
/*            PredatorBird               */
/**************************************/

PredatorBird::PredatorBird() : Bird() {

}

int PredatorBird::get_bird_type() {
  return 1;
}

double PredatorBird::get_max_speed() {
  return PRED_MAX_SPEED;
}






