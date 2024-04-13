#include "constants.h"

#include "position.h"

#ifndef GOOD_H
#define GOOD_H

class Good {
public:
	int id;
	Position pos;
	int value;
    int app_time;
    bool available;
    int dis[constants::len_env][constants::len_env];
    int pursuer;
    Good() {}
    Good(int _id, Position _pos, int _value, int _app_time):
        id(_id), pos(_pos), value(_value), app_time(_app_time) {
            available = true;
            pursuer = -1;
        }
};

#endif