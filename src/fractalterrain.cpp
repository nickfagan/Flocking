#include "fractalterrain.hpp"
#include "constants.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include "a2.hpp"

FractalTerrain::FractalTerrain() {
}

void FractalTerrain::initialize(bool mode_all) {
  m_mode_all = mode_all;

  if(!m_mode_all) return;

  m_mountains = new double*[8];

  mountain_side_planes = new Point3D*[4];
  mountain_side_normals = new Vector3D[4];
  mountain_corner_planes = new Point3D*[8];
  mountain_corner_normals = new Vector3D[8];

  m_dimension = pow(2, ITERATIONS) + 1;
  m_num_vertices = (m_dimension)*(m_dimension);

  int i;
  for(i = 0; i < 8; i++) {
    m_mountains[i] = new double[m_num_vertices];
    mountain_corner_planes[i] = new Point3D[3];
  }

  for(i = 0; i < 4; i++) {
    m_mountains[i] = new double[m_num_vertices];
    mountain_side_planes[i] = new Point3D[4];
  }

  // Init the heights of the 8 mountain segments
  init_mountain(m_mountains[0], MOUNTAIN_HEIGHT, MOUNTAIN_HEIGHT, MOUNTAIN_HEIGHT, 0);
  init_mountain(m_mountains[1], MOUNTAIN_HEIGHT, MOUNTAIN_HEIGHT, 0, 0);
  init_mountain(m_mountains[2], MOUNTAIN_HEIGHT, MOUNTAIN_HEIGHT, 0, MOUNTAIN_HEIGHT);
  init_mountain(m_mountains[3], MOUNTAIN_HEIGHT, 0, MOUNTAIN_HEIGHT, 0);
  init_mountain(m_mountains[4], 0, MOUNTAIN_HEIGHT, 0, MOUNTAIN_HEIGHT);
  init_mountain(m_mountains[5], MOUNTAIN_HEIGHT, 0, MOUNTAIN_HEIGHT, MOUNTAIN_HEIGHT);
  init_mountain(m_mountains[6], 0, 0, MOUNTAIN_HEIGHT, MOUNTAIN_HEIGHT);
  init_mountain(m_mountains[7], 0, MOUNTAIN_HEIGHT, MOUNTAIN_HEIGHT, MOUNTAIN_HEIGHT);

  generate_terrain();

  // Calculate the planes of the mountains to use for collision detection and their normals. This
  // definitly could be refactored but it works and theres no time so its staying like this

  double a = ((m_dimension-1) + (m_dimension-1)/2);
  double b = ((m_dimension-1)/2);
  double translate[][3] = {{-a, 0, -a}, {-b, 0, -a}, {b, 0, -a}, {-a, 0, -b}, {b, 0, -b}, {-a, 0, b}, {-b, 0, b}, {b, 0, b}};

  int top_left = 0;
  int top_right = m_dimension-1;
  int bottom_left = m_num_vertices - m_dimension;
  int bottom_right = m_num_vertices - 1;

  Vector3D u, v;

  mountain_corner_planes[0][0] = get_world_coords(Point3D(0, m_mountains[0][bottom_left], m_dimension-1), translate[0]);
  mountain_corner_planes[0][1] = get_world_coords(Point3D(0, m_mountains[0][top_left], 0), translate[0]);
  mountain_corner_planes[0][2] = get_world_coords(Point3D(m_dimension-1, m_mountains[0][top_right], 0), translate[0]);
  mountain_corner_planes[1][0] = get_world_coords(Point3D(0, m_mountains[0][bottom_left], m_dimension-1), translate[0]);
  mountain_corner_planes[1][1] = get_world_coords(Point3D(m_dimension-1, m_mountains[0][top_right], 0), translate[0]);
  mountain_corner_planes[1][2] = get_world_coords(Point3D(m_dimension-1, m_mountains[0][bottom_right], m_dimension-1), translate[0]);

  u = mountain_corner_planes[0][1] - mountain_corner_planes[0][0];
  v = mountain_corner_planes[0][1] - mountain_corner_planes[0][2];
  mountain_corner_normals[0] = u.cross(v);
  mountain_corner_normals[0].normalize();

  u = mountain_corner_planes[1][1] - mountain_corner_planes[1][0];
  v = mountain_corner_planes[1][1] - mountain_corner_planes[1][2];
  mountain_corner_normals[1] = u.cross(v);
  mountain_corner_normals[1].normalize();

  mountain_corner_planes[2][0] = get_world_coords(Point3D(0, m_mountains[2][top_left], 0), translate[2]);
  mountain_corner_planes[2][1] = get_world_coords(Point3D(m_dimension-1, m_mountains[2][top_right], 0), translate[2]);
  mountain_corner_planes[2][2] = get_world_coords(Point3D(m_dimension-1, m_mountains[2][bottom_right], m_dimension-1), translate[2]);
  mountain_corner_planes[3][0] = get_world_coords(Point3D(0, m_mountains[2][top_left], 0), translate[2]);
  mountain_corner_planes[3][1] = get_world_coords(Point3D(m_dimension-1, m_mountains[2][bottom_right], m_dimension-1), translate[2]);
  mountain_corner_planes[3][2] = get_world_coords(Point3D(0, m_mountains[2][bottom_left], m_dimension-1), translate[2]);

  u = mountain_corner_planes[2][1] - mountain_corner_planes[2][0];
  v = mountain_corner_planes[2][1] - mountain_corner_planes[2][2];
  mountain_corner_normals[2] = u.cross(v);
  mountain_corner_normals[2].normalize();

  u = mountain_corner_planes[3][1] - mountain_corner_planes[3][0];
  v = mountain_corner_planes[3][1] - mountain_corner_planes[3][2];
  mountain_corner_normals[3] = u.cross(v);
  mountain_corner_normals[3].normalize();

  mountain_corner_planes[4][0] = get_world_coords(Point3D(0, m_mountains[7][top_left], 0), translate[7]);
  mountain_corner_planes[4][1] = get_world_coords(Point3D(m_dimension-1, m_mountains[7][top_right], 0), translate[7]);
  mountain_corner_planes[4][2] = get_world_coords(Point3D(0, m_mountains[7][bottom_left], m_dimension-1), translate[7]);
  mountain_corner_planes[5][0] = get_world_coords(Point3D(m_dimension-1, m_mountains[7][top_right], 0), translate[7]);
  mountain_corner_planes[5][1] = get_world_coords(Point3D(m_dimension-1, m_mountains[7][bottom_right], m_dimension-1), translate[7]);
  mountain_corner_planes[5][2] = get_world_coords(Point3D(0, m_mountains[7][bottom_left], m_dimension-1), translate[7]);

  u = mountain_corner_planes[4][0] - mountain_corner_planes[4][2];
  v = mountain_corner_planes[4][0] - mountain_corner_planes[4][1];
  mountain_corner_normals[4] = u.cross(v);
  mountain_corner_normals[4].normalize();

  u = mountain_corner_planes[5][0] - mountain_corner_planes[5][2];
  v = mountain_corner_planes[5][0] - mountain_corner_planes[5][1];
  mountain_corner_normals[5] = u.cross(v);
  mountain_corner_normals[5].normalize();

  mountain_corner_planes[6][0] = get_world_coords(Point3D(0, m_mountains[5][top_left], 0), translate[5]);
  mountain_corner_planes[6][1] = get_world_coords(Point3D(m_dimension-1, m_mountains[5][top_right], 0), translate[5]);
  mountain_corner_planes[6][2] = get_world_coords(Point3D(m_dimension-1, m_mountains[5][bottom_right], m_dimension-1), translate[5]);
  mountain_corner_planes[7][0] = get_world_coords(Point3D(0, m_mountains[5][top_left], 0), translate[5]);
  mountain_corner_planes[7][1] = get_world_coords(Point3D(m_dimension-1, m_mountains[5][bottom_right], m_dimension-1), translate[5]);
  mountain_corner_planes[7][2] = get_world_coords(Point3D(0, m_mountains[5][bottom_left], m_dimension-1), translate[5]);

  u = mountain_corner_planes[6][0] - mountain_corner_planes[6][2];
  v = mountain_corner_planes[6][0] - mountain_corner_planes[6][1];
  mountain_corner_normals[6] = u.cross(v);
  mountain_corner_normals[6].normalize();

  u = mountain_corner_planes[7][0] - mountain_corner_planes[7][2];
  v = mountain_corner_planes[7][0] - mountain_corner_planes[7][1];
  mountain_corner_normals[7] = u.cross(v);
  mountain_corner_normals[7].normalize();

  mountain_side_planes[0][0] = get_world_coords(Point3D(0, m_mountains[1][top_left], 0), translate[1]);
  mountain_side_planes[0][1] = get_world_coords(Point3D(m_dimension-1, m_mountains[1][top_right], 0), translate[1]);
  mountain_side_planes[0][2] = get_world_coords(Point3D(m_dimension-1, m_mountains[1][bottom_right], m_dimension-1), translate[1]);
  mountain_side_planes[0][3] = get_world_coords(Point3D(0, m_mountains[1][bottom_left], m_dimension-1), translate[1]);

  u = mountain_side_planes[0][0] - mountain_side_planes[0][2];
  v = mountain_side_planes[0][0] - mountain_side_planes[0][1];
  mountain_side_normals[0] = u.cross(v);
  mountain_side_normals[0].normalize();

  mountain_side_planes[1][0] = get_world_coords(Point3D(0, m_mountains[4][top_left], 0), translate[4]);
  mountain_side_planes[1][1] = get_world_coords(Point3D(m_dimension-1, m_mountains[4][top_right], 0), translate[4]);
  mountain_side_planes[1][2] = get_world_coords(Point3D(m_dimension-1, m_mountains[4][bottom_right], m_dimension-1), translate[4]);
  mountain_side_planes[1][3] = get_world_coords(Point3D(0, m_mountains[4][bottom_left], m_dimension-1), translate[4]);

  u = mountain_side_planes[1][0] - mountain_side_planes[1][2];
  v = mountain_side_planes[1][0] - mountain_side_planes[1][1];
  mountain_side_normals[1] = u.cross(v);
  mountain_side_normals[1].normalize();

  mountain_side_planes[2][0] = get_world_coords(Point3D(0, m_mountains[6][top_left], 0), translate[6]);
  mountain_side_planes[2][1] = get_world_coords(Point3D(m_dimension-1, m_mountains[6][top_right], 0), translate[6]);
  mountain_side_planes[2][2] = get_world_coords(Point3D(m_dimension-1, m_mountains[6][bottom_right], m_dimension-1), translate[6]);
  mountain_side_planes[2][3] = get_world_coords(Point3D(0, m_mountains[6][bottom_left], m_dimension-1), translate[6]);

  u = mountain_side_planes[2][0] - mountain_side_planes[2][2];
  v = mountain_side_planes[2][0] - mountain_side_planes[2][1];
  mountain_side_normals[2] = u.cross(v);
  mountain_side_normals[2].normalize();

  mountain_side_planes[3][0] = get_world_coords(Point3D(0, m_mountains[3][top_left], 0), translate[3]);
  mountain_side_planes[3][1] = get_world_coords(Point3D(m_dimension-1, m_mountains[3][top_right], 0), translate[3]);
  mountain_side_planes[3][2] = get_world_coords(Point3D(m_dimension-1, m_mountains[3][bottom_right], m_dimension-1), translate[3]);
  mountain_side_planes[3][3] = get_world_coords(Point3D(0, m_mountains[3][bottom_left], m_dimension-1), translate[3]);

  u = mountain_side_planes[3][0] - mountain_side_planes[3][2];
  v = mountain_side_planes[3][0] - mountain_side_planes[3][1];
  mountain_side_normals[3] = u.cross(v);
  mountain_side_normals[3].normalize();
}

void FractalTerrain::init_mountain(double *mountain, double h1, double h2, double h3, double h4) {
  int top_left = 0;
  int top_right = m_dimension-1;
  int bottom_left = m_num_vertices - m_dimension;
  int bottom_right = m_num_vertices - 1;

  mountain[top_left] = h1;
  mountain[top_right] = h2;
  mountain[bottom_left] = h3;
  mountain[bottom_right] = h4;
}

Point3D FractalTerrain::get_world_coords(Point3D p, double local_translate[]) {
  Matrix4x4 trans;

  double scale = 2.0*WORLD_BOUNDS/((m_dimension-1)*3.0);

  trans = trans*translation(Vector3D(0, -WORLD_BOUNDS, 0));
  trans = trans*scaling(Vector3D(scale, scale, scale));
  trans = trans*translation(Vector3D(local_translate[0], local_translate[1], local_translate[2]));

  return trans*p;
}

void FractalTerrain::set_mountain_color() {
  if(rand()%101 < 20) {
    double snow_color = (rand()%16 + 80)/100.0;
    GLfloat mat_diffuse[] = {snow_color, snow_color, snow_color};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  } else {
    GLfloat mat_diffuse[] = {0.3, 0.3, 0.3};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  }
}

void FractalTerrain::render() {
  int m_index, i, j;

  if(!m_mode_all) return;

  if(dl_index == 0) {
    int m_index, i, j;
    double a = ((m_dimension-1) + (m_dimension-1)/2);
    double b = ((m_dimension-1)/2);
    double translate[][3] = {{-a, 0, -a}, {-b, 0, -a}, {b, 0, -a}, {-a, 0, -b}, {b, 0, -b}, {-a, 0, b}, {-b, 0, b}, {b, 0, b}};

    double scale = 2.0*WORLD_BOUNDS/((m_dimension-1)*3.0);

    // Generate a new display list
    dl_index = glGenLists(1);
    glNewList(dl_index, GL_COMPILE);

    // Set the material
    GLfloat mat_specular[] = {0.2, 0.2, 0.2};
    GLfloat mat_shininess[] = { 10 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glPushMatrix();

    // Translate all 8 sections of the mountain down and scale them to fit the world
    glTranslatef(0, -WORLD_BOUNDS, 0);
    glScalef(scale, scale, scale);

    // Draw the plane grey floor
    draw_floor();

    for(m_index = 0; m_index < 8; m_index++) {

      glPushMatrix();

      glTranslatef(translate[m_index][0], translate[m_index][1], translate[m_index][2]);

      for(i = 0; i < m_dimension-1; i++) {
        for(j = 0; j < m_dimension-1; j++) {
          int p1i = i*m_dimension + j;
          int p2i = i*m_dimension + (j+1);
          int p3i = (i+1)*m_dimension + (j+1);
          int p4i = (i+1)*m_dimension + j;
 
          Point3D p1 = Point3D(j, m_mountains[m_index][p1i], i);
          Point3D p2 = Point3D(j+1, m_mountains[m_index][p2i], i);
          Point3D p3 = Point3D(j+1, m_mountains[m_index][p3i], i+1);
          Point3D p4 = Point3D(j, m_mountains[m_index][p4i], i+1);

          Vector3D n1, n2, u, v;

          u = p3 - p1;
          v = p2 - p1;
          n1 = u.cross(v);

          u = p4 - p1;
          v = p3 - p1;
          n2 = u.cross(v);

          glBegin(GL_TRIANGLES);
          set_mountain_color();
          glNormal3f(n1[0], n1[1], n1[2]);
          glVertex3f(p1[0], p1[1], p1[2]);
          glVertex3f(p2[0], p2[1], p2[2]);
          glVertex3f(p3[0], p3[1], p3[2]);

          set_mountain_color();
          glNormal3f(n2[0], n2[1], n2[2]);
          glVertex3f(p1[0], p1[1], p1[2]);
          glVertex3f(p3[0], p3[1], p3[2]);
          glVertex3f(p4[0], p4[1], p4[2]);
          glEnd();
        }
      }

      glPopMatrix();
    }

    glPopMatrix();
    glEndList();

    // No longer need the mountians height data
    free_mountains();
  } else {
    glCallList(dl_index);
  }
}

void FractalTerrain::destroy() {
  int i;

  for(i = 0; i < 8; i++) {
    delete [] mountain_corner_planes[i];
  }

  for(i = 0; i < 4; i++) {
    delete [] mountain_side_planes[i];
  }

  delete [] mountain_side_planes;
  delete [] mountain_side_normals;
  delete [] mountain_corner_planes;
  delete [] mountain_corner_normals;
}

void FractalTerrain::generate_terrain() {
  int m_index, i, j;
  
  int step = (m_dimension-1)/2.0;
  double ratio = (double) pow (2,-MOUNTAIN_ROUGHNESS);
  double scale = HEIGHT_RANGE * ratio;

  for(i = 0; i < ITERATIONS; i++) {
    fractalize(step, scale);

    scale = scale*ratio;
    step = step/2;
  }
}

void FractalTerrain::fractalize(int step, double scale) {
  int m_index, i, j;
  bool even_column;

  for(m_index = 0; m_index < 8; m_index++) {
    for (i = step; i < (m_dimension-1); i += 2*step) {
      for (j = step; j < (m_dimension-1); j += 2*step) {
        m_mountains[m_index][(i * m_dimension) + j] = square_height(m_index, i, j, step, scale);
      }
    }
  }

  for(m_index = 0; m_index < 8; m_index++) {
    even_column = false;

    for (i = 0; i < m_dimension; i+= step) {
      even_column = !even_column;

      for (j = 0; j < m_dimension; j += 2*step) {
        if (even_column && j == 0) {
          j = step;
        }
        m_mountains[m_index][(i * m_dimension) + j] = diamond_height(m_index, i, j, step, scale);
      }
    }
  }
}

double FractalTerrain::square_height(int m_index, int i, int j, int step, double scale) {
  double top_left = m_mountains[m_index][((i-step)*m_dimension) + j-step];
  double top_right = m_mountains[m_index][((i-step)*m_dimension) + j+step];
  double bottom_left = m_mountains[m_index][((i+step)*m_dimension) + j-step];
  double bottom_right = m_mountains[m_index][((i+step)*m_dimension) + j+step];

  double avg = (top_left + top_right + bottom_left + bottom_right)/4.0;

  double random = ((double)(rand() % 101 - 50))/100.0;

  return scale*random + avg;
}

double FractalTerrain::diamond_height(int m_index, int i, int j, int step, double scale) {
  double h1, h2, h3, h4;
  double edge_height;
  bool apply_random = false;

  if(i == 0) {						// North mountain edge
    apply_random = get_north_mountain_edge_height(m_index, j, edge_height);
  } else if (i == m_dimension-1) { 			// South mountain edge
    apply_random = get_south_mountain_edge_height(m_index, j, step, edge_height);
  } else if (j == 0) { 					// West mountain edge
    apply_random = get_west_mountain_edge_height(m_index, i, edge_height);
  } else if (j == m_dimension-1) { 			//East mountain edge
    apply_random = get_east_mountain_edge_height(m_index, i, step, edge_height);
  } else { 						// Inside mountain
    h1 = m_mountains[m_index][((i-step)*m_dimension) + j];
    h2 = m_mountains[m_index][((i+step)*m_dimension) + j];
    h3 = m_mountains[m_index][(i*m_dimension) + j-step];
    h4 = m_mountains[m_index][(i*m_dimension) + j+step];

    edge_height = (h1+h2+h3+h4)/4.0;

    apply_random = true;
  }
    
  double random = ((double)(rand() % 101 - 50))/100.0;

  if(apply_random) {
    edge_height += scale*random;
  }

  return edge_height;
}

bool FractalTerrain::get_north_mountain_edge_height(int m_index, int j, double &edge_height) {
  switch(m_index) {
    case 0:
      edge_height = MOUNTAIN_HEIGHT;
      return false;
    case 1:
      edge_height = MOUNTAIN_HEIGHT;
      return false;
    case 2:
      edge_height = MOUNTAIN_HEIGHT;
      return false;
    case 3:
      edge_height = m_mountains[0][m_dimension*(m_dimension-1) + j];
      return false;
    case 4:
      edge_height = m_mountains[2][m_dimension*(m_dimension-1) + j];
      return false;
    case 5:
      edge_height = m_mountains[3][m_dimension*(m_dimension-1) + j];
      return false;
    case 6:
      edge_height = 0;
      return true;
    case 7:
      edge_height = m_mountains[4][m_dimension*(m_dimension-1) + j];
      return false;
  }

  return false;
}

bool FractalTerrain::get_south_mountain_edge_height(int m_index, int j, int step, double &edge_height) {
  int i = m_dimension - 1;
  double h1, h2, h3, h4;
      
  h1 = m_mountains[m_index][((i-step)*m_dimension) + j];
  h2 = m_mountains[m_index][(i*m_dimension) + j-step];
  h3 = m_mountains[m_index][(i*m_dimension) + j+step];

  switch(m_index) {
    case 0:
      h4 = m_mountains[3][(step*m_dimension) + j];
      edge_height = (h1+h2+h3+h4)/4.0;
      return true;
    case 1:
      edge_height = 0;
      return true;
    case 2:
      h4 = m_mountains[4][(step*m_dimension) + j];
      edge_height = (h1+h2+h3+h4)/4.0;
      return true;
    case 3:
      h4 = m_mountains[5][(step*m_dimension) + j];
      edge_height = (h1+h2+h3+h4)/4.0;
      return true;
    case 4:
      h4 = m_mountains[7][(step*m_dimension) + j];
      edge_height = (h1+h2+h3+h4)/4.0;
      return true;
    case 5:
      edge_height = MOUNTAIN_HEIGHT;
      return false;
    case 6:
      edge_height = MOUNTAIN_HEIGHT;
      return false;
    case 7:
      edge_height = MOUNTAIN_HEIGHT;
      return false;
  }
  
  return false;
}

bool FractalTerrain::get_west_mountain_edge_height(int m_index, int i, double &edge_height) {
  switch(m_index) {
    case 0:
      edge_height = MOUNTAIN_HEIGHT;
      return false;
    case 1:
      edge_height = m_mountains[0][i*m_dimension + (m_dimension - 1)];
      return false;
    case 2:
      edge_height = m_mountains[1][i*m_dimension + (m_dimension - 1)];
      return false;
    case 3:
      edge_height = MOUNTAIN_HEIGHT;
      return false;
    case 4:
      edge_height = 0;
      return true;
    case 5:
      edge_height = MOUNTAIN_HEIGHT;
      return false;
    case 6:
      edge_height = m_mountains[5][i*m_dimension + (m_dimension - 1)];
      return false;
    case 7:
      edge_height = m_mountains[6][i*m_dimension + (m_dimension - 1)];
      return false;
  }
}

bool FractalTerrain::get_east_mountain_edge_height(int m_index, int i, int step, double &edge_height) {
  int j = m_dimension - 1;
  double h1, h2, h3, h4;
      
  h1 = m_mountains[m_index][((i-step)*m_dimension) + j];
  h3 = m_mountains[m_index][((i+step)*m_dimension) + j];
  h2 = m_mountains[m_index][(i*m_dimension) + j-step];

  switch(m_index) {
    case 0:
      h4 = m_mountains[1][(i*m_dimension) + step];
      edge_height = (h1+h2+h3+h4)/4.0;
      return true;
    case 1:
      h4 = m_mountains[2][(i*m_dimension) + step];
      edge_height = (h1+h2+h3+h4)/4.0;
      return true;
    case 2:
      edge_height = MOUNTAIN_HEIGHT;
      return false;
    case 3:
      edge_height = 0;
      return true;
    case 4:
      edge_height = MOUNTAIN_HEIGHT;
      return false;
    case 5:
      h4 = m_mountains[6][(i*m_dimension) + step];
      edge_height = (h1+h2+h3+h4)/4.0;
      return true;
    case 6:
      h4 = m_mountains[7][(i*m_dimension) + step];
      edge_height = (h1+h2+h3+h4)/4.0;
      return true;
    case 7:
      edge_height = MOUNTAIN_HEIGHT;
      return false;
  }
}

void FractalTerrain::free_mountains() {
  int i;
  for(i = 0; i < 8; i++) {
    delete [] m_mountains[i];
  }

  delete [] m_mountains;
}

void FractalTerrain::detect_collision(Bird *bird) {
  int i;

  if(!m_mode_all) return;

  for(i = 0; i < 8; i++) {
    Vector3D normal = mountain_corner_normals[i];
    Point3D p1 = mountain_corner_planes[i][0];
    Point3D p2 = mountain_corner_planes[i][1];
    Point3D p3 = mountain_corner_planes[i][2];

    Vector3D bird_direction = bird->get_velocity();
    bird_direction.normalize();

    double facing = normal.dot(bird_direction);

    // If the bird is not heading in the direction of the plane the mountain surface
    // lines on then we do not need to consider it.
    if(facing >= 0) continue;

    // Get the intersection point with the plane
    double t = -normal.dot(bird->get_position() - p1) / facing;
    Point3D intersection_point = bird->get_position() + t * bird->get_velocity();
    Vector3D to_point = bird->get_position() - intersection_point;

    // get the distance to the intersection point
    double distance = to_point.magnitude();

    // If its greater than our collision distnace then we dont care if this point is on
    // the mountian plane or not since we wont considier it either way
    if(distance > COLLISION_DISTANCE) continue;

    // So the bird is going to collide with the plane the mountain plane is part of
    // now we need to see if the intersection point is actaully on the mountain plane.

    Vector3D point_to_plane = intersection_point - p1;
    Vector3D v0 = p2 - p1;
    Vector3D v1 = p3 - p1;

    double dot2 = v0.dot(v0);
    double dot3 = v0.dot(v1);
    double dot4 = v1.dot(v1);
    double dot5 = point_to_plane.dot(v0);
    double dot6 = point_to_plane.dot(v1);
    double denom = dot3 * dot3 - dot2 * dot4;

    double a, b;
    a = (dot3 * dot6 - dot4 * dot5) / denom;
    if (a < 0.0 || a > 1.0)        
        continue;
    b = (dot3 * dot5 - dot2 * dot6) / denom;
    if (b < 0.0 || (a + b) > 1.0) 
        continue;

    bird->add_velocity((1.0/distance)*normal);
  }

  for(i = 0; i < 4; i++) {
    Vector3D normal = mountain_side_normals[i];
    Point3D p1 = mountain_side_planes[i][0];
    Point3D p2 = mountain_side_planes[i][1];
    Point3D p3 = mountain_side_planes[i][2];
    Point3D p4 = mountain_side_planes[i][3];

    double facing = normal.dot(bird->get_velocity());

    // If the bird is not heading in the direction of the plane the mountain surface
    // lines on then we do not need to consider it.
    if(facing >= 0) continue;

    double t = -normal.dot(bird->get_position() - p1) / facing;
    Point3D i_point = bird->get_position() + t * bird->get_velocity();

    // See if the point is in the x/z plane of the mountain plane
    if(!(i_point[0] >= p1[0] && i_point[0] <= p2[0] &&
       i_point[2] >= p1[2] && i_point[2] <= p3[2])) {
      continue;
    }

    Vector3D to_point = bird->get_position() - i_point;
    double distance = to_point.magnitude();

    if(distance <= COLLISION_DISTANCE) {
      bird->add_velocity((1.0/distance)*normal);
    }
  }
}

void FractalTerrain::draw_floor() {
    double edge = ((m_dimension-1) + (m_dimension-1)/2);

    GLfloat mat_diffuse[] = {0.3, 0.3, 0.3};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);

    glVertex3f(-3000, MOUNTAIN_HEIGHT, -3000);
    glVertex3f(-3000, MOUNTAIN_HEIGHT, 3000);
    glVertex3f(-edge, MOUNTAIN_HEIGHT, 3000);
    glVertex3f(-edge, MOUNTAIN_HEIGHT, -3000);

    glVertex3f(-edge, MOUNTAIN_HEIGHT, edge);
    glVertex3f(-edge, MOUNTAIN_HEIGHT, 3000);
    glVertex3f(edge, MOUNTAIN_HEIGHT, 3000);
    glVertex3f(edge, MOUNTAIN_HEIGHT, edge);

    glVertex3f(edge, MOUNTAIN_HEIGHT, 3000);
    glVertex3f(edge, MOUNTAIN_HEIGHT, -3000);
    glVertex3f(3000, MOUNTAIN_HEIGHT, 3000);
    glVertex3f(3000, MOUNTAIN_HEIGHT, -3000);

    glVertex3f(-edge, MOUNTAIN_HEIGHT, -edge);
    glVertex3f(-edge, MOUNTAIN_HEIGHT, -3000);
    glVertex3f(edge, MOUNTAIN_HEIGHT, -3000);
    glVertex3f(edge, MOUNTAIN_HEIGHT, -edge);
    glEnd();
}














