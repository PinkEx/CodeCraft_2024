#include <queue>

#include "constants.h"

#include "position.h"

#ifndef ROBOT_H
#define ROBOT_H

const int cand_good = 3;
const int cand_berth = 1;
const double G = 0.2;

class Robot {
public:
    int id;
    Position pos;
    int good_taken;
    int last_dir;
    int last_time;
    int matched_good;
    std::vector <int> target_berths;
    std::vector <int> target_goods;
    Robot() {}
    Robot(int _id, Position _pos):
        id(_id), pos(_pos) {
            good_taken = -1;
            last_dir = -1;
            last_time = 1;
            matched_good = -1;
            target_goods.clear();
        }
};

#endif