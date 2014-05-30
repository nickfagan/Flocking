#ifndef PROJECT_HPP
#define PROJECT_HPP

#include "flock.hpp"
#include "fractalterrain.hpp"
#include "binlattice.hpp"
#include "icelake.hpp"
#include "algebra.hpp"
#include "skybox.hpp"
#include "bird.hpp"
#include "constants.hpp"

class Project {
public:
  Project();

  void update();
  void render();
  bool initialize(int argc, char** argv);
  void destroy();

  void switch_binlattice();

private:
  void render_lake_reflections();
  double* get_clip_plane(Point3D p1, Point3D p2, Point3D p3);
  void draw_world_bounds();

private:
  bool m_mode_all;
  bool m_binlattice_on;
  BinLattice m_bin_lattice;

  SkyBox m_skybox;
  FractalTerrain m_fractal_terrain;
  IceLake m_ice_lake;

  PreyFlock m_prey_flock;
  PredatorFlock m_predator_flock;

  double* m_icelake_clipe_plane;
};

#endif
