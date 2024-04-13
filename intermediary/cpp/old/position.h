#ifndef POSITION_H
#define POSITION_H
class Position {
public:
    int x, y;
    Position() {}
    Position(int _x, int _y) : x(_x), y(_y) {}
    bool operator == (const Position &o) const {
        return x == o.x && y == o.y;
    }
};
#endif