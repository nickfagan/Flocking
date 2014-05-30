#ifndef BIN_LATTICE_HPP
#define BIN_LATTICE_HPP

#include <list>
#include "bin.hpp"
#include "algebra.hpp"

class BinLattice {
public:
  BinLattice();

  void initialize();
  void draw();
  void destroy();

  std::list<Bin*> get_neighbour_bins(Bin *bin, double distance);
  Bin* get_bin(double x, double y, double z);
  Bin* get_new_bin(Point3D position, Bin* old_bin);
private:
  int get_bin_index(int i, int j, int k);

private:
  Bin **m_bins;
};

#endif
