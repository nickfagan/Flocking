#ifndef FLOCK_HPP
#define FLOCK_HPP

#include "algebra.hpp"
#include "bird.hpp"
#include "fractalterrain.hpp"
#include "binlattice.hpp"
#include "icelake.hpp"
#include <list>

class Flock {
public:
  Flock();

  void initialize(bool mode_all, int bird_type, int flock_size, double local_dist, double sep_dist, double sep_scale, double ali_scale, double coh_scale, BinLattice bin_lattice);
  void render();
  void destroy();

  std::list<Bird*> get_flock();
  void turn_binlattice_on(bool on);

protected:
  bool calc_reynolds(Bird *bird, std::list<Bird*> birds);
  void detect_world_bound_collisions(Bird *bird);
  void detect_wall_collision(Bird *bird, Point3D wall_points[4], Vector3D normal);

protected:
  bool m_turn_on_binlattice;

  Point3D m_world_bounds[8];
  Vector3D m_world_bounds_normals[6];

  Bird* m_target;

  std::list<Bird*> m_birds;

  int m_flock_size;
  double m_local_dist;
  double m_sep_dist;
  double m_sep_scale;
  double m_ali_scale;
  double m_coh_scale;

  int m_bird_id;

};

class PreyFlock : public Flock {
public:
  PreyFlock();

  void update(Flock predator_flock, FractalTerrain fractal_terrain, IceLake ice_lake);

private:
  void flee(Bird *bird, std::list<Bird*> predator_birds);
};

class PredatorFlock : public Flock {
public:
  PredatorFlock();

  void update(Flock prey_flock, FractalTerrain fractal_terrain, IceLake ice_lake);

private:
  void chase_prey(Bird* bird, std::list<Bird*> prey_birds);
};

#endif
