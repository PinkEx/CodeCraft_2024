#include "constants.h"

#include "position.h"

#ifndef GOOD_H
#define GOOD_H

class Good {
public:
	int id;
	Position pos;
	int value;
    bool available;
    int dis[constants::len_env][constants::len_env];
    int pursuer;
    Good() {}
    Good(int _id, Position _pos, int _value):
        id(_id), pos(_pos), value(_value) {
            available = true;
            pursuer = -1;
        }
};

#endif