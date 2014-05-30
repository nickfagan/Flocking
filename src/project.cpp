#include <iostream>
#include <cstdlib>
#include <time.h>
#include <string>

#include "project.hpp"

Project::Project() {

}

bool Project::initialize(int argc, char** argv) {
  bool result;
  int prey_flock_size, pred_flock_size;

  if(argc < 3) {
    std::cout << "Usage: ./flocking prey_flock_size predator_flock_size [-mode all|basic]\n\nDescription:\n\"mode\" sets what will be included in the flocking program, options are:\n - \"all\" : runs program with all features.\n - \"basic\" : turns off all but the basic flocking features.\nDefault is \"all\"" << std::endl;
    return false;
  }
  
  srand (time(NULL));
  m_binlattice_on = true;

  prey_flock_size = atoi(argv[1]);
  pred_flock_size = atoi(argv[2]);

  m_mode_all = true;

  if(argc > 3) {
    std::string mode = argv[4];
    if(mode == "basic") m_mode_all = false;
  }

  m_bin_lattice.initialize();

  m_prey_flock.initialize(m_mode_all, 0, prey_flock_size, PREY_LOCAL_DIST, PREY_SEP_DIST, PREY_SEP_SCALE, PREY_ALI_SCALE, PREY_COH_SCALE, m_bin_lattice);
  m_predator_flock.initialize(m_mode_all, 1, pred_flock_size, PRED_LOCAL_DIST, PRED_SEP_DIST, PRED_SEP_SCALE, PRED_ALI_SCALE, PRED_COH_SCALE, m_bin_lattice);

  m_skybox.initialize(m_mode_all);
  m_fractal_terrain.initialize(m_mode_all);
  m_ice_lake.initialize(m_mode_all);

  m_icelake_clipe_plane = m_ice_lake.get_clip_plane();

  return true;
}

void Project::update() {
  m_prey_flock.update(m_predator_flock, m_fractal_terrain, m_ice_lake);
  m_predator_flock.update(m_prey_flock, m_fractal_terrain, m_ice_lake);
}

void Project::render() {
  draw_world_bounds();

  render_lake_reflections();

  // Draw the actual lake
  m_ice_lake.render();

  // Draw eveything thing else
  m_skybox.render();
  m_fractal_terrain.render();
  m_prey_flock.render();
  m_predator_flock.render();
}

void Project::render_lake_reflections() {
  if(!m_mode_all) return;

  // Disable some stuff
  glDisable(GL_DEPTH_TEST);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  // Enable stencil testing
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
  
  // Draw the lake to set the stencil buffer
  m_ice_lake.render();

  // re-enable some stuff
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  // Tell open gl to only draw pixels for the stencil
  glStencilFunc(GL_EQUAL, 1, 0xffffffff); 
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  // Set up a clip plane to clip anything above the lake (for the reflections)
  glClipPlane(GL_CLIP_PLANE0, m_icelake_clipe_plane);
  glEnable(GL_CLIP_PLANE0);

  // Draw the scene reflected by the lake
  glPushMatrix();
  m_ice_lake.reflect();
  m_skybox.render();
  m_fractal_terrain.render();
  m_prey_flock.render();
  m_predator_flock.render();
  glPopMatrix();

  // Disable the clip plane and stencil test
  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_STENCIL_TEST);
}

double* Project::get_clip_plane(Point3D p1, Point3D p2, Point3D p3) {
  double* eq = new double[4];
  eq[0] = (p1[1]*(p2[2] - p3[2])) + (p2[1]*(p3[2] - p1[2])) + (p3[1]*(p1[2] - p2[2]));
  eq[1] = (p1[2]*(p2[0] - p3[0])) + (p2[2]*(p3[0] - p1[0])) + (p3[2]*(p1[0] - p2[0]));
  eq[2] = (p1[0]*(p2[1] - p3[1])) + (p2[0]*(p3[1] - p1[1])) + (p3[0]*(p1[1] - p2[1]));
  eq[3] = -((p1[0]*((p2[1]*p3[2]) - (p3[1]*p2[2]))) + (p2[0]*((p3[1]*p1[2]) - (p1[1]*p3[2]))) + (p3[0]*((p1[1]*p2[2]) - (p2[1]*p1[2]))));

  return eq;
}

void Project::draw_world_bounds() {
  GLfloat mat_diffuse[] = {1, 1, 1};
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

  glBegin(GL_LINES);
  glVertex3f(-WORLD_BOUNDS, -WORLD_BOUNDS, -WORLD_BOUNDS);
  glVertex3f(-WORLD_BOUNDS, WORLD_BOUNDS, -WORLD_BOUNDS);
  glVertex3f(-WORLD_BOUNDS, WORLD_BOUNDS, -WORLD_BOUNDS);
  glVertex3f(WORLD_BOUNDS, WORLD_BOUNDS, -WORLD_BOUNDS);
  glVertex3f(WORLD_BOUNDS, WORLD_BOUNDS, -WORLD_BOUNDS);
  glVertex3f(WORLD_BOUNDS, -WORLD_BOUNDS, -WORLD_BOUNDS);
  glVertex3f(WORLD_BOUNDS, -WORLD_BOUNDS, -WORLD_BOUNDS);
  glVertex3f(-WORLD_BOUNDS, -WORLD_BOUNDS, -WORLD_BOUNDS);


  glVertex3f(-WORLD_BOUNDS, -WORLD_BOUNDS, -WORLD_BOUNDS);
  glVertex3f(-WORLD_BOUNDS, -WORLD_BOUNDS, WORLD_BOUNDS);
  glVertex3f(-WORLD_BOUNDS, WORLD_BOUNDS, -WORLD_BOUNDS);
  glVertex3f(-WORLD_BOUNDS, WORLD_BOUNDS, WORLD_BOUNDS);
  glVertex3f(WORLD_BOUNDS, WORLD_BOUNDS, -WORLD_BOUNDS);
  glVertex3f(WORLD_BOUNDS, WORLD_BOUNDS, WORLD_BOUNDS);
  glVertex3f(WORLD_BOUNDS, -WORLD_BOUNDS, -WORLD_BOUNDS);
  glVertex3f(WORLD_BOUNDS, -WORLD_BOUNDS, WORLD_BOUNDS);

  glVertex3f(-WORLD_BOUNDS, -WORLD_BOUNDS, WORLD_BOUNDS);
  glVertex3f(-WORLD_BOUNDS, WORLD_BOUNDS, WORLD_BOUNDS);
  glVertex3f(-WORLD_BOUNDS, WORLD_BOUNDS, WORLD_BOUNDS);
  glVertex3f(WORLD_BOUNDS, WORLD_BOUNDS, WORLD_BOUNDS);
  glVertex3f(WORLD_BOUNDS, WORLD_BOUNDS, WORLD_BOUNDS);
  glVertex3f(WORLD_BOUNDS, -WORLD_BOUNDS, WORLD_BOUNDS);
  glVertex3f(WORLD_BOUNDS, -WORLD_BOUNDS, WORLD_BOUNDS);
  glVertex3f(-WORLD_BOUNDS, -WORLD_BOUNDS, WORLD_BOUNDS);
  glEnd();
}

void Project::destroy() {
  m_bin_lattice.destroy();
  m_prey_flock.destroy();
  m_predator_flock.destroy();
 
  if(m_mode_all) {
    m_fractal_terrain.destroy();
  }
}

void Project::switch_binlattice() {
  m_binlattice_on = !m_binlattice_on;

  m_prey_flock.turn_binlattice_on(m_binlattice_on);
  m_predator_flock.turn_binlattice_on(m_binlattice_on);
}












