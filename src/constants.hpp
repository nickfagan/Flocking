#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

// World bounds, birds will not fly greater than WORLD_BOUNDS or less 
// than -WORLD_BOUNDS in the x, y, z directions
static const int WORLD_BOUNDS = 100;

// Prey flock size
static const int PREY_FS = 100;
// Prey bird local distance
static const double PREY_LOCAL_DIST =  20;
// Prey seperation distance
static const double PREY_SEP_DIST = 8;
// Prey scales for Reymonds rules
// Increase SEP to make the birds spread out more, generally, the
// larger local dist you have the large sep you need or birds start
// clumping together hard stylez
static const double PREY_SEP_SCALE = 1.8;
static const double PREY_ALI_SCALE = 0.2;
static const double PREY_COH_SCALE = 0.01;
// Prey birds max speed
static const double PREY_MAX_SPEED = 1.0;

/* Predator bird/flock variables, these variables will make the preds fly in
   small tight flocks */

// Predator flock size
static const int PRED_FS = 10;
// Predator bird local distance
static const double PRED_LOCAL_DIST =  20;
// Predator seperation distance
static const double PRED_SEP_DIST = 10;
// Predator scales for Reymonds rules
static const double PRED_SEP_SCALE = 1.0;
static const double PRED_ALI_SCALE = 0.5;
static const double PRED_COH_SCALE = 0.01;
// Predator birds max speed
static const double PRED_MAX_SPEED = 1.4;
// Predator birds cruising speed
static const double PRED_CRUIS_SPEED = 1.2;

/*****************************/
/* Fractal Terrain Constants */
/*****************************/

static const int ITERATIONS = 7;
static const int MOUNTAIN_HEIGHT = 40;
static const double MOUNTAIN_ROUGHNESS = 0.8;
static const double HEIGHT_RANGE = 30;
static const double COLLISION_DISTANCE = 20;

static const int BIN_COUNT = 10+2;

#endif
