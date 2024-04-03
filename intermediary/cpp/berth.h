#include <queue>

#include "constants.h"

#include "position.h"

#ifndef BERTH_H
#define BERTH_H

class Berth {
public:
    int id;
    Position pos;
    int velocity;
    int occupied;
    int reserved;
    int land_dis[constants::len_env][constants::len_env];
    int water_dis[4][constants::len_env][constants::len_env];
    std::queue <int> received_goods;
    Berth() {}
    Berth(int _id, Position _pos, int _velocity) :
        id(_id), pos(_pos), velocity(_velocity) {
            occupied = -1;
            reserved = -1;
        }
};

#endif