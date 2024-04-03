#include <vector>

#include "constants.h"

#include "berth.h"
#include "boat.h"
#include "good.h"
#include "robot.h"

#ifndef VARIABLES_H
#define VARIABLES_H

// time
extern int last_purchase_robot;
extern int last_purchase_boat;

// money
extern int money, sum_value;

// environment
extern char env[constants::len_env][constants::len_env];
extern int which_berth[constants::len_env][constants::len_env];
extern int which_boat[constants::len_env][constants::len_env];
extern int which_good[constants::len_env][constants::len_env];
extern int which_robot[constants::len_env][constants::len_env];
extern std::vector <Position> terminals;

// berth
extern std::vector <Berth> berths;

// boat
extern int boat_capacity, load_threshold;
extern std::vector <Boat> boats;
extern std::vector <Position> boat_spawn_points;

// good
extern std::vector <Good> goods;

// robot
extern std::vector <Robot> robots;
extern std::vector <Position> robot_spawn_points;

// terminal
extern int dis_terminal[4][constants::len_env][constants::len_env];

#endif