#ifndef BIRD_HPP
#define BIRD_HPP

#include <list>
#include "algebra.hpp"
#include "binlattice.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

class Bin;

class Bird {
public:
  Bird();

  void initialize(bool mode_all, int id, Point3D position, Vector3D velocity, double local_dist, BinLattice bin_lattice);
  void update();
  void render();

  Point3D get_position();
  Point3D set_position(Point3D new_position){m_position = new_position;}

  Vector3D get_velocity();
  void add_velocity(Vector3D velocity);

  void set_bins();
  std::list<Bin*> get_bins(){return m_bins;}
  void turn_binlattice_on(bool on);

  int get_id() {return m_id;}

  virtual int get_bird_type() = 0;
  virtual double get_max_speed() = 0;

  /* DEBUG functions */
  void set_target(bool target) {m_target = target;}
  bool get_target() {return m_target;}
  void set_targets_local(bool targets_local) {m_targets_local = targets_local;}
  bool get_targets_local() {return m_targets_local;}

private:
  void draw_bird();
  void draw_basic_bird();
  void set_bird_material();

protected:
  BinLattice m_bin_lattice;
  std::list<Bin*> m_bins;
  Bin* m_bin;

private:
  struct Direction {
    double x_rotation;
    double y_rotation;
  };
  int m_id;

  bool m_mode_all;
  bool m_binlattice_on;

  GLuint dl_index1;
  GLuint dl_index2;
  GLuint dl_index3;

  Point3D t_points[5];
  Vector3D t_normals[5];

  Point3D m_position;
  Vector3D m_velocity;
  Direction m_direction;

  double m_local_dist;

  double m_wing_angle;
  double m_wa_inc;

  /* DEBUG variables */
  bool m_target;
  bool m_targets_local;
};

class PreyBird : public Bird {
public:
  PreyBird();

  virtual int get_bird_type();
  virtual double get_max_speed();
};

class PredatorBird : public Bird {
public:
  PredatorBird();

  virtual int get_bird_type();
  virtual double get_max_speed();
};

#endif
