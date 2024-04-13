#include <string>
#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace constants {
    const int len_env = 200;
    const int robot_cost[2] = {2000, 5000};
    const int boat_cost = 8000;
    const int mv[4][2] = {
        {0, 1}, // R
        {0, -1}, // L
        {-1, 0}, // U
        {1, 0} // D
    };
    const int rot[2][4][2] = {
        // dir=R, L, U, D
        {{0, 2}, {0, -2}, {-2, 0}, {2, 0}}, // clockwise
        {{1, 1}, {-1, -1}, {-1, 1}, {1, -1}} // anti-clockwise
    };
    const int rel_to_core[4][6][2] = {
        // part=core, side_core, rot, side_rot, front, side_front
        {{0, 0}, {1, 0}, {0, 1}, {1, 1}, {0, 2}, {1, 2}}, // R
        {{0, 0}, {-1, 0}, {0, -1}, {-1, -1}, {0, -2}, {-1, -2}}, // L
        {{0, 0}, {0, 1}, {-1, 0}, {-1, 1}, {-2, 0}, {-2, 1}}, // U
        {{0, 0}, {0, -1}, {1, 0}, {1, -1}, {2, 0}, {2, -1}} // D
    };
    const int inf = 0x3f3f3f3f;
    const double ninf = -1e7;
    const std::string robot_zone_flag = ".C>RcB";
    const std::string boat_zone_flag = "*C~SKTcB";
}

#endif