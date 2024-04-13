#include <queue>

#include "constants.h"

#include "position.h"

#ifndef BOAT_H
#define BOAT_H

class Boat {
public:
    int id;
    Position pos;
    int load;
    int d;
    int status;
    int target;
    Boat() {}
    Boat(int _id, Position _pos, int _load = 0, int _d = 0, int _status = 0):
        id(_id), pos(_pos), load(_load), d(_d), status(_status) {
            target = -1;
        }
    inline bool enough_load(int bound) {
        return load >= bound;
    }
};

#endif