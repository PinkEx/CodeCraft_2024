#include "variables.h"

// time
int last_purchase_robot = constants::ninf;
int last_purchase_boat = constants::ninf;

// money
int money, sum_value;

// environment
char env[constants::len_env][constants::len_env];
int which_berth[constants::len_env][constants::len_env];
int which_good[constants::len_env][constants::len_env];
int which_robot[constants::len_env][constants::len_env];
std::vector <Position> terminals;

// berth
std::vector <Berth> berths;

// boat
int boat_capacity;
std::vector <Boat> boats;
std::vector <Position> boat_spawn_points;

// good
std::vector <Good> goods;

// robot
std::vector <Robot> robots;
std::vector <Position> robot_spawn_points;

// terminal
int dis_terminal[4][constants::len_env][constants::len_env];