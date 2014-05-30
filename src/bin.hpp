#ifndef BIN_HPP
#define BIN_HPP

#include <iostream>
#include <list>
#include "algebra.hpp"

class Bird;

class Bin {
public:
  Bin();
  Bin(int i, int j, int k, double x, double y, double z, double size) {
    m_i = i;
    m_j = j;
    m_k = k;
    m_x = x;
    m_y = y;
    m_z = z;
    m_size = size;
  }

  int i(){return m_i;}
  int j(){return m_j;}
  int k(){return m_k;}

  double x(){return m_x;}
  double y(){return m_y;}
  double z(){return m_z;}

  double size(){return m_size;}

  void add_bird(Bird* bird, int type) {
    if(type == 0) {
      m_prey_birds.push_front(bird);
    } else {
      m_pred_birds.push_front(bird);
    }
  }

  void remove_bird(Bird* bird, int type) {
    if(type == 0) {
      m_prey_birds.remove(bird);
    } else {
      m_pred_birds.remove(bird);
    }
  }

  void add_mountain_piece(Point3D *piece){m_mountain_pieces.push_front(piece);}

  std::list<Bird*> get_birds(int type) {
    if(type == 0) {
      return m_prey_birds;
    } else {
      return m_pred_birds;
    }
  }

  std::list<Point3D*> get_mountain_pieces() {return m_mountain_pieces;}

private:
  int m_i, m_j, m_k;
  double m_x, m_y, m_z;
  double m_size;

  std::list<Bird*> m_prey_birds;
  std::list<Bird*> m_pred_birds;
  std::list<Point3D*> m_mountain_pieces;
};

#endif
