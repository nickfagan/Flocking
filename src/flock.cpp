#include <iostream>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>

#include "flock.hpp"
#include "algebra.hpp"
#include "constants.hpp"


Flock::Flock() {
}

void Flock::initialize(bool mode_all, int bird_type, int flock_size, double local_dist, double sep_dist, double sep_scale, double ali_scale, double coh_scale, BinLattice bin_lattice) {
  int i;

  m_flock_size = flock_size;
  m_local_dist = local_dist;
  m_sep_dist = sep_dist;
  m_sep_scale = sep_scale;
  m_ali_scale = ali_scale;
  m_coh_scale = coh_scale;
  m_turn_on_binlattice = true;

  m_bird_id = 0;

  for(i = 0; i < m_flock_size; i++) {
    Bird *new_bird;

    if(bird_type == 0) {
      new_bird = new PreyBird();
    } else {
      new_bird = new PredatorBird();
    }

    if(i == 0) new_bird->set_target(true);
    else new_bird->set_target(false);

    double dx = rand() % 200 - 100;
    double dy = rand() % 200 - 100;
    double dz = rand() % 200 - 100;

    int min = -WORLD_BOUNDS + MOUNTAIN_HEIGHT + 10;
    int max = WORLD_BOUNDS;

    double x = rand() % (WORLD_BOUNDS + WORLD_BOUNDS) - WORLD_BOUNDS;
    double y = rand() % (max - min + 1) + min;
    double z = rand() % (WORLD_BOUNDS + WORLD_BOUNDS) - WORLD_BOUNDS;

    Point3D position = Point3D(x, y, z);
    Vector3D velocity = Vector3D(dx, dy, dz);

    velocity.normalize();


    new_bird->initialize(mode_all, m_bird_id, position, velocity, m_local_dist, bin_lattice);
    m_birds.push_front(new_bird);

    m_bird_id++;
  }

  m_world_bounds[0] = Point3D(-WORLD_BOUNDS, -WORLD_BOUNDS, WORLD_BOUNDS);
  m_world_bounds[1] = Point3D(-WORLD_BOUNDS, WORLD_BOUNDS, WORLD_BOUNDS);
  m_world_bounds[2] = Point3D(WORLD_BOUNDS, WORLD_BOUNDS, WORLD_BOUNDS);
  m_world_bounds[3] = Point3D(WORLD_BOUNDS, -WORLD_BOUNDS, WORLD_BOUNDS);
  m_world_bounds[4] = Point3D(-WORLD_BOUNDS, -WORLD_BOUNDS, -WORLD_BOUNDS);
  m_world_bounds[5] = Point3D(-WORLD_BOUNDS, WORLD_BOUNDS, -WORLD_BOUNDS);
  m_world_bounds[6] = Point3D(WORLD_BOUNDS, WORLD_BOUNDS, -WORLD_BOUNDS);
  m_world_bounds[7] = Point3D(WORLD_BOUNDS, -WORLD_BOUNDS, -WORLD_BOUNDS);

  m_world_bounds_normals[0] = Vector3D(1, 0, 0);
  m_world_bounds_normals[1] = Vector3D(-1, 0, 0);
  m_world_bounds_normals[2] = Vector3D(0, 1, 0);
  m_world_bounds_normals[3] = Vector3D(0, -1, 0);
  m_world_bounds_normals[4] = Vector3D(0, 0, 1);
  m_world_bounds_normals[5] = Vector3D(0, 0, -1);
}

void Flock::destroy() {
  for (std::list<Bird*>::iterator it=m_birds.begin(); it != m_birds.end(); ++it) {
    delete *it;
  }
}

void Flock::detect_world_bound_collisions(Bird *bird) {
  int i;

  Point3D p1 = m_world_bounds[0];
  Point3D p2 = m_world_bounds[1];
  Point3D p3 = m_world_bounds[2];
  Point3D p4 = m_world_bounds[3];
  Point3D p5 = m_world_bounds[4];
  Point3D p6 = m_world_bounds[5];
  Point3D p7 = m_world_bounds[6];
  Point3D p8 = m_world_bounds[7];

  Point3D walls[][4] = {{p6, p2, p1, p5},
			{p3, p7, p8, p4},
			{p1, p4, p8, p5},
			{p6, p7, p3, p2},
                        {p7, p6, p5, p8},
			{p2, p3, p4, p1}};

  for(i = 0; i < 6; i++) {
    detect_wall_collision(bird, walls[i], m_world_bounds_normals[i]);
  }
}

void Flock::detect_wall_collision(Bird *bird, Point3D wall_points[4], Vector3D normal) {
  Point3D p1 = wall_points[0];
  Point3D p2 = wall_points[1];
  Point3D p3 = wall_points[2];
  Point3D p4 = wall_points[3];

  double facing = normal.dot(bird->get_velocity());

  // If the bird is not heading in the direction of the plane the mountain surface
  // lines on then we do not need to consider it.
  if(facing >= 0) return;

  double t = -normal.dot(bird->get_position() - p1) / facing;
  Point3D i_point = bird->get_position() + t * bird->get_velocity();

  Vector3D to_point = bird->get_position() - i_point;
  double distance = to_point.magnitude();

  if(distance <= COLLISION_DISTANCE) {
    bird->add_velocity((1.0/distance)*normal);
  }
}

void Flock::render() {
  for (std::list<Bird*>::iterator it=m_birds.begin(); it != m_birds.end(); ++it) {
    Bird *bird = (Bird*) *it;
    
    bird->render();
  }
}

bool Flock::calc_reynolds(Bird *bird, std::list<Bird*> birds) {
  Vector3D sep = Vector3D();
  Vector3D ali = Vector3D();
  Vector3D coh = Vector3D();

  Point3D center = Point3D();

  double sep_count = 0;
  double local_count = 0;

  bool found_other_bird = false;

  for (std::list<Bird*>::iterator it=birds.begin(); it != birds.end(); ++it) {
    Bird *other_bird = (Bird*) *it;
    
    if(other_bird == bird) continue;

    Point3D bird_pos = bird->get_position();
    Point3D other_bird_pos = other_bird->get_position();

    double x_diff = bird_pos[0] - other_bird_pos[0];
    double y_diff = bird_pos[1] - other_bird_pos[1];
    double z_diff = bird_pos[2] - other_bird_pos[2];
    double distance = sqrt(x_diff*x_diff + y_diff*y_diff + z_diff*z_diff);

    if(distance != 0 && distance <= m_local_dist) {
      found_other_bird = true;
      local_count++;

      ali = ali + other_bird->get_velocity();
      center = center + other_bird_pos;

      if(bird->get_target()) other_bird->set_targets_local(true);

    }

    if(distance != 0 && distance <= m_sep_dist) {
      found_other_bird = true;
      sep_count++;

      Vector3D v = bird_pos - other_bird_pos;
      v.normalize();
      v = (1.0/distance)*v;

      sep = sep + v;
    }
  }

  if(local_count > 0) {
    center = (1.0/local_count)*center;
    coh = center - bird->get_position();
    ali = (1.0/local_count)*ali;
  }

  if(sep_count > 0) {
    sep = (1.0/sep_count)*sep;
  }

  bird->add_velocity(m_sep_scale*sep + m_ali_scale*ali + m_coh_scale*coh);

  return found_other_bird;
}

std::list<Bird*> Flock::get_flock() {
  return m_birds;
}
  
void Flock::turn_binlattice_on(bool on) {
  m_turn_on_binlattice = on;

  for (std::list<Bird*>::iterator it=m_birds.begin(); it != m_birds.end(); ++it) {
    Bird *bird = (Bird*) *it;
    bird->turn_binlattice_on(on);
  }
}

/**************************************/
/*            PreyFlock               */
/**************************************/

			     //local_dist, spe_dist, sep_scale, ali_scale, coh_scale, world_bounds
PreyFlock::PreyFlock() : Flock() {
}

void PreyFlock::update(Flock predator_flock, FractalTerrain fractal_terrain, IceLake ice_lake) {
  std::list<Bird*> predator_birds = predator_flock.get_flock();

  for (std::list<Bird*>::iterator it=m_birds.begin(); it != m_birds.end(); ++it) {
    Bird *bird = (Bird*) *it;

    std::list<Bird*> local_birds;
    std::list<Bird*> pred_birds;
    std::list<Point3D*> local_mountain_pieces;

    if(m_turn_on_binlattice) {
      std::list<Bin*> bins = bird->get_bins();
      for (std::list<Bin*>::iterator it=bins.begin(); it != bins.end(); ++it) {
        Bin *bin = (Bin*) *it;

        std::list<Bird*> l_birds = bin->get_birds(0);
        std::list<Bird*> p_birds = bin->get_birds(1);
        std::list<Point3D*> mountain_pieces = bin->get_mountain_pieces();

        local_birds.splice(local_birds.end(), l_birds);
        pred_birds.splice(pred_birds.end(), p_birds);
        local_mountain_pieces.splice(local_mountain_pieces.end(), mountain_pieces);
      }
    } else {
      local_birds = m_birds;
      pred_birds = predator_birds;
    }

    bool result;

    result = calc_reynolds(bird, local_birds);
    if(!result) {
      bird->add_velocity(bird->get_velocity());
    }

    detect_world_bound_collisions(bird);

    flee(bird, pred_birds);

    fractal_terrain.detect_collision(bird);
    ice_lake.detect_collision(bird);

    bird->update();
  }

}

void PreyFlock::flee(Bird *bird, std::list<Bird*> predator_birds) {
  Vector3D pred_v = Vector3D();
  Point3D pred_loc = Point3D();
  double count = 0;

  for (std::list<Bird*>::iterator it=predator_birds.begin(); it != predator_birds.end(); ++it) {
    PredatorBird *predator_bird = (PredatorBird*) *it;

    Point3D bird_pos = bird->get_position();
    Point3D pred_bird_pos = predator_bird->get_position();

    double x_diff = bird_pos[0] - pred_bird_pos[0];
    double y_diff = bird_pos[1] - pred_bird_pos[1];
    double z_diff = bird_pos[2] - pred_bird_pos[2];
    double distance = sqrt(x_diff*x_diff + y_diff*y_diff + z_diff*z_diff);

    if(distance > m_local_dist) continue;

    count++;

    pred_loc = pred_loc + pred_bird_pos;
  }

  if(count > 0) {
    pred_loc = (1.0/count)*pred_loc;
    pred_v = bird->get_position() - pred_loc;

    double distance = pred_v.magnitude();

    if(distance != 0) {
      pred_v = (1/distance)*pred_v;
    }
  }

  bird->add_velocity(0.8*pred_v);
}

/**************************************/
/*          PredatorFlock             */
/**************************************/

PredatorFlock::PredatorFlock() : Flock() {
}

void PredatorFlock::update(Flock prey_flock, FractalTerrain fractal_terrain, IceLake ice_lake) {
  for (std::list<Bird*>::iterator it=m_birds.begin(); it != m_birds.end(); ++it) {
    Bird *bird = (Bird*) *it;


    std::list<Bird*> prey_birds;
    std::list<Bird*> local_birds;
    std::list<Point3D*> local_mountain_pieces;

    if(m_turn_on_binlattice) {
      std::list<Bin*> bins = bird->get_bins();
      for (std::list<Bin*>::iterator it=bins.begin(); it != bins.end(); ++it) {
        Bin *bin = (Bin*) *it;

        std::list<Bird*> l_birds = bin->get_birds(1);
        std::list<Bird*> p_birds = bin->get_birds(0);
        std::list<Point3D*> mountain_pieces = bin->get_mountain_pieces();

        local_birds.splice(local_birds.end(), l_birds);
        prey_birds.splice(prey_birds.end(), p_birds);
        local_mountain_pieces.splice(local_mountain_pieces.end(), mountain_pieces);
      }
    } else {
      local_birds = m_birds;
      prey_birds = prey_flock.get_flock();
    }

    bool result;

    result = calc_reynolds(bird, local_birds);
    if(!result) {
      bird->add_velocity(bird->get_velocity());
    }

    chase_prey(bird, prey_birds);
    detect_world_bound_collisions(bird);
    fractal_terrain.detect_collision(bird);
    ice_lake.detect_collision(bird);

    bird->update();

  }
}

void PredatorFlock::chase_prey(Bird* bird, std::list<Bird*> prey_birds) {
  Bird* closest_bird = NULL;
  double closest_dist = INT_MAX;

  for (std::list<Bird*>::iterator it=prey_birds.begin(); it != prey_birds.end(); ++it) {
    Bird *prey_bird = (Bird*) *it;

    Point3D bird_pos = bird->get_position();
    Point3D prey_bird_pos = prey_bird->get_position();

    double x_diff = bird_pos[0] - prey_bird_pos[0];
    double y_diff = bird_pos[1] - prey_bird_pos[1];
    double z_diff = bird_pos[2] - prey_bird_pos[2];
    double distance = sqrt(x_diff*x_diff + y_diff*y_diff + z_diff*z_diff);

    if(distance > m_local_dist && distance > closest_dist) continue;
    
    closest_dist = distance;
    closest_bird = prey_bird;
  }

  if(closest_bird != NULL) {
    Vector3D to_bird = closest_bird->get_position() - bird->get_position();

    if(closest_dist <= 2.0) {
      closest_bird->set_position(Point3D(0, 0, 0));
    } else {
      bird->add_velocity(0.5*to_bird);
    }
  }
}




