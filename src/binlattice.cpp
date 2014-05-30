#include "binlattice.hpp"
#include "constants.hpp"
#include <cmath>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include "bird.hpp"

BinLattice::BinLattice() {
}

void BinLattice::initialize() {
  m_bins = new Bin*[BIN_COUNT*BIN_COUNT*BIN_COUNT];

  double bin_size = (double)WORLD_BOUNDS*2.0/(double)(BIN_COUNT-2);
  int count = 0;
  int i, j, k;
  for(i = 0; i < BIN_COUNT; i++) {
    for(j = 0; j < BIN_COUNT; j++) {
      for(k = 0; k < BIN_COUNT; k++) {
        int index = get_bin_index(i, j, k);
        double x = bin_size*i + bin_size/2.0 - WORLD_BOUNDS - bin_size;
        double y = bin_size*j + bin_size/2.0 - WORLD_BOUNDS - bin_size;
        double z = bin_size*k + bin_size/2.0 - WORLD_BOUNDS - bin_size;

        Bin *bin = new Bin(i, j, k, x, y, z, bin_size);
        m_bins[index] = bin;
      }
    }
  }
}

void BinLattice::destroy() {
  int i, j, k;

  for(i = 0; i < BIN_COUNT; i++) {
    for(j = 0; j < BIN_COUNT; j++) {
      for(k = 0; k < BIN_COUNT; k++) {
        int index = get_bin_index(i, j, k);
        Bin *bin = m_bins[index];
        delete bin;
      }
    }
  }

  delete [] m_bins;
}

void BinLattice::draw() {
  int i, j, k;
for(i = 0; i < BIN_COUNT; i++) {
    for(j = 0; j < BIN_COUNT; j++) {
      for(k = 0; k < BIN_COUNT; k++) {
        int index = get_bin_index(i, j, k);
        Bin *bin = m_bins[index];

        double x = bin->x();
        double y = bin->y();
        double z = bin->z();

        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        
          glVertex3f(bin->x() - bin->size()/2, bin->y() - bin->size()/2, bin->z() - bin->size()/2);
          glVertex3f(bin->x() - bin->size()/2, bin->y() + bin->size()/2, bin->z() - bin->size()/2);
          glVertex3f(bin->x() - bin->size()/2, bin->y() + bin->size()/2, bin->z() - bin->size()/2);
          glVertex3f(bin->x() + bin->size()/2, bin->y() + bin->size()/2, bin->z() - bin->size()/2);
          glVertex3f(bin->x() + bin->size()/2, bin->y() + bin->size()/2, bin->z() - bin->size()/2);
          glVertex3f(bin->x() + bin->size()/2, bin->y() - bin->size()/2, bin->z() - bin->size()/2);
          glVertex3f(bin->x() + bin->size()/2, bin->y() - bin->size()/2, bin->z() - bin->size()/2);
          glVertex3f(bin->x() - bin->size()/2, bin->y() - bin->size()/2, bin->z() - bin->size()/2);

          glVertex3f(bin->x() - bin->size()/2, bin->y() - bin->size()/2, bin->z() + bin->size()/2);
          glVertex3f(bin->x() - bin->size()/2, bin->y() + bin->size()/2, bin->z() + bin->size()/2);
          glVertex3f(bin->x() - bin->size()/2, bin->y() + bin->size()/2, bin->z() + bin->size()/2);
          glVertex3f(bin->x() + bin->size()/2, bin->y() + bin->size()/2, bin->z() + bin->size()/2);
          glVertex3f(bin->x() + bin->size()/2, bin->y() + bin->size()/2, bin->z() + bin->size()/2);
          glVertex3f(bin->x() + bin->size()/2, bin->y() - bin->size()/2, bin->z() + bin->size()/2);
          glVertex3f(bin->x() + bin->size()/2, bin->y() - bin->size()/2, bin->z() + bin->size()/2);
          glVertex3f(bin->x() - bin->size()/2, bin->y() - bin->size()/2, bin->z() + bin->size()/2);

          glVertex3f(bin->x() - bin->size()/2, bin->y() - bin->size()/2, bin->z() - bin->size()/2);
          glVertex3f(bin->x() - bin->size()/2, bin->y() - bin->size()/2, bin->z() + bin->size()/2);
          glVertex3f(bin->x() - bin->size()/2, bin->y() + bin->size()/2, bin->z() - bin->size()/2);
          glVertex3f(bin->x() - bin->size()/2, bin->y() + bin->size()/2, bin->z() + bin->size()/2);
          glVertex3f(bin->x() + bin->size()/2, bin->y() + bin->size()/2, bin->z() - bin->size()/2);
          glVertex3f(bin->x() + bin->size()/2, bin->y() + bin->size()/2, bin->z() + bin->size()/2);
          glVertex3f(bin->x() + bin->size()/2, bin->y() - bin->size()/2, bin->z() - bin->size()/2);
          glVertex3f(bin->x() + bin->size()/2, bin->y() - bin->size()/2, bin->z() + bin->size()/2);

        glEnd();
      }
    }
  }
}

Bin* BinLattice::get_bin(double x, double y, double z) {
  int i, j, k;
  for(i = 0; i < BIN_COUNT; i++) {
    for(j = 0; j < BIN_COUNT; j++) {
      for(k = 0; k < BIN_COUNT; k++) {
        int index = get_bin_index(i, j, k);

        Bin *bin = m_bins[index];

        if(x >= bin->x() - bin->size()/2.0 && x <= bin->x() + bin->size()/2.0 &&
           y >= bin->y() - bin->size()/2.0 && y <= bin->y() + bin->size()/2.0 &&
           z >= bin->z() - bin->size()/2.0 && z <= bin->z() + bin->size()/2.0) {
          return bin;
        }
      }
    }
  }

  return 0;
}

Bin* BinLattice::get_new_bin(Point3D position, Bin* old_bin) {
  int new_i, new_j, new_k;
  bool new_bin;

  new_i = old_bin->i();
  new_j = old_bin->j();
  new_k = old_bin->k();
  new_bin = false;

  if(position[0] < old_bin->x() - old_bin->size()/2.0) {
    new_i -= 1; 
    new_bin = true;
  } else if(position[0] > old_bin->x() + old_bin->size()/2.0) {
    new_i += 1;
    new_bin = true;
  }

  if(position[1] < old_bin->y() - old_bin->size()/2.0) {
    new_j -= 1; 
    new_bin = true;
  } else if(position[1] > old_bin->y() + old_bin->size()/2.0) {
    new_j += 1;
    new_bin = true;
  }

  if(position[2] < old_bin->z() - old_bin->size()/2.0) {
    new_k -= 1; 
    new_bin = true;
  } else if(position[2] > old_bin->z() + old_bin->size()/2.0) {
    new_k += 1;
    new_bin = true;
  }

  if(new_bin) {
    return m_bins[get_bin_index(new_i, new_j, new_k)];
  }

  return NULL;
}

std::list<Bin*> BinLattice::get_neighbour_bins(Bin *bin, double distance) {
  std::list<Bin*> neighbour_bins;

  double levels = ceil((distance - bin->size()/2.0)/bin->size());

  int i, j, k;
  int i_min, i_max, j_min, j_max, k_min, k_max;

  i_min = (bin->i() - levels < 0) ? 0 : bin->i() - levels;
  i_max = (bin->i() + levels > BIN_COUNT - 1) ? BIN_COUNT - 1 : bin->i() + levels;
  j_min = (bin->j() - levels < 0) ? 0 : bin->j() - levels;
  j_max = (bin->j() + levels > BIN_COUNT - 1) ? BIN_COUNT - 1 : bin->j() + levels;
  k_min = (bin->k() - levels < 0) ? 0 : bin->k() - levels;
  k_max = (bin->k() + levels > BIN_COUNT - 1) ? BIN_COUNT - 1 : bin->k() + levels;

  for(i = i_min; i < i_max + 1; i++) {
    for(j = j_min; j < j_max + 1; j++) {
      for(k = k_min; k < k_max + 1; k++) {
        int index = get_bin_index(i, j, k);

        double x_diff = m_bins[index]->x() - bin->x();
        double y_diff = m_bins[index]->y() - bin->y();
        double z_diff = m_bins[index]->z() - bin->z();
        double box_distance = sqrt(x_diff*x_diff + y_diff*y_diff + z_diff*z_diff);

        if(distance > box_distance - bin->size()) {
          neighbour_bins.push_front(m_bins[index]);
        }
      }
    }
  }

 // std::cout << "return here3" << std::endl;
  return neighbour_bins;
}

int BinLattice::get_bin_index(int i, int j, int k) {
  if(i < 0) i = 0;
  if(i > BIN_COUNT) i = BIN_COUNT-1;
  if(j < 0) j = 0;
  if(j > BIN_COUNT) j = BIN_COUNT-1;
  if(k < 0) k = 0;
  if(k > BIN_COUNT) k = BIN_COUNT-1;

  return i*BIN_COUNT*BIN_COUNT + j*BIN_COUNT + k;
}











