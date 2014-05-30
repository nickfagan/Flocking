#ifndef FRACTAL_TERRAIN_HPP
#define FRACTAL_TERRAIN_HPP

#include "algebra.hpp"
#include "bird.hpp"
#include "binlattice.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

class FractalTerrain {
public:
  FractalTerrain();

  void initialize(bool mode_all);
  void destroy();
  void render();

  void detect_collision(Bird *bird);

private:
  void init_mountain(double *mountain, double h1, double h2, double h3, double h4);
  void generate_terrain();
  void fractalize(int step, double scale);
  double square_height(int m_index, int i, int j, int step, double scale);
  double diamond_height(int m_index, int i, int j, int step, double scale);

  bool get_north_mountain_edge_height(int m_index, int i, double &edge_height);
  bool get_south_mountain_edge_height(int m_index, int i, int step, double &edge_height);
  bool get_west_mountain_edge_height(int m_index, int j, double &edge_height);
  bool get_east_mountain_edge_height(int m_index, int j, int step, double &edge_height);

  Point3D get_world_coords(Point3D p, double local_translate[]);

  void draw_floor();

  void set_mountain_color();

  void free_mountains();

private:
  bool m_mode_all;
  double **m_mountains;

  // Used for collision detection
  Point3D **mountain_side_planes;
  Vector3D *mountain_side_normals;
  Point3D **mountain_corner_planes;
  Vector3D *mountain_corner_normals;

  int m_num_vertices;
  int m_dimension;

  double m_scale;
  GLuint dl_index;
};

#endif
