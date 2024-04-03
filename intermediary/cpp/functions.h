#include <algorithm>

#include "constants.h"
#include "variables.h"

#include "berth.h"
#include "good.h"
#include "robot.h"

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// map
inline bool outside_map(Position pos) {
    return pos.x < 0 || pos.x >= constants::len_env
        || pos.y < 0 || pos.y >= constants::len_env;
}
inline bool robot_zone(Position pos) {
    return std::count(constants::robot_zone_flag.begin(), constants::robot_zone_flag.end(), env[pos.x][pos.y]);
}
inline bool boat_zone(Position pos) {
    return std::count(constants::boat_zone_flag.begin(), constants::boat_zone_flag.end(), env[pos.x][pos.y]);
}
inline bool rapid_zone(Position pos) {
    return env[pos.x][pos.y] == '*' || env[pos.x][pos.y] == 'C';
}
inline bool robot_spawn_point(Position pos) {
    return env[pos.x][pos.y] == 'R';
}
inline bool boat_spawn_point(Position pos) {
    return env[pos.x][pos.y] == 'S';
}
inline bool berth_or_dock_point(Position pos) {
    return env[pos.x][pos.y] == 'B' or env[pos.x][pos.y] == 'K';
}

// interactions
void confirm_ok(bool in = true, bool out = true) {
    static char okk[10];
    if (in) {
        scanf("%s", okk);
    }
    if (out) {
        printf("%s\n", okk);
        fflush(stdout);
    }
}
void lbot(Position pos) {
    if (money < constants::robot_cost) return;
    printf("lbot %d %d\n", pos.x, pos.y);
    fflush(stdout);
    robots.emplace_back(Robot(robots.size(), pos));
}
void lboat(Position pos) {
    if (money < constants::boat_cost) return;
    printf("lboat %d %d\n", pos.x, pos.y);
    fflush(stdout);
    boats.emplace_back(Boat(boats.size(), pos));
}

// berth
void load(Berth &berth) {
    for (int i = 0; i < berth.velocity; i++) {
        if (!berth.received_goods.empty() && !boats[berth.occupied].enough_load(boat_capacity * 0.75)) {
            berth.received_goods.pop();
            boats[berth.occupied].load += 1;
        } else return;
    }
}

// robot
int get(Robot &r) {
    if (~r.good_taken) return -1;
    if (which_good[r.pos.x][r.pos.y] == -1) return -1;
    r.good_taken = which_good[r.pos.x][r.pos.y];
    r.last_dir = -1;
    return r.good_taken;
}

void pull(Robot &r, int &good_id, int &berth_id) {
    if (r.good_taken == -1) {
        good_id = -1;
        berth_id = -1;
        return;
    }
    if (env[r.pos.x][r.pos.y] != 'B') {
        good_id = -1;
        berth_id = -1;
        return;
    }
    good_id = r.good_taken;
    berth_id = which_berth[r.pos.x][r.pos.y];
    r.good_taken = -1;
    r.last_dir = -1;
}

double eval_goods(Position ori, Position pos, std::vector <int> target_goods) {
	double score = 0, coe = 1, delta; int order = 0;
	for (auto good_id: target_goods) {
		delta = 1.0 / (goods[good_id].dis[pos.x][pos.y] + 0.5)
            - 1.0 / (goods[good_id].dis[ori.x][ori.y] + 0.5);
        if (delta > 0) score += delta * coe;
        order++, coe *= G;
	}
	return score;
}

double eval_berths(Position ori, Position pos, std::vector <int> target_berths) {
	double score = 0, coe = 1, delta; int order = 0;
	for (auto berth_id: target_berths) {
		delta = 1.0 / (berths[berth_id].land_dis[pos.x][pos.y] + 0.5)
            - 1.0 / (berths[berth_id].land_dis[ori.x][ori.y] + 0.5);
        if (delta > 0) score += delta * coe;
        order++, coe *= G;
	}
	return score;
}

double move(const Robot &r, int d) {
	int x = r.pos.x + constants::mv[d][0], y = r.pos.y + constants::mv[d][1];
    if (outside_map(Position(x, y))) return constants::ninf;
    if (!robot_zone(Position(x, y))) return constants::ninf;
    if (which_robot[x][y] >= 0) return constants::ninf;
    double score = 0;
    if (~r.good_taken) {
        score = eval_berths(r.pos, Position(x, y), r.target_berths);
    } else {
        score = eval_goods(r.pos, Position(x, y), r.target_goods);
    }
	if (r.last_dir == d) score *= 1.0 + pow(0.01, (r.last_time)); // encourage to keep diretcion
	else if ((r.last_dir ^ d) == 1) score *= 0.01; // discourage to turn back
    return score;
}

// boat
inline int new_direction(int d, int r) { // r: 0-clockwise, 1-anti-clockwise
    if (d == 0) return r ? 2 : 3;
    else if (d == 1) return r ? 3 : 2;
    else if (d == 2) return r ? 1 : 0;
    else if (d == 3) return r ? 0 : 1;
}

int time_cost(Position pos, int d) {
    static Position p;
    for (int i = 0; i < 6; i++) {
        p = Position(
            pos.x + constants::rel_to_core[d][i][0],
            pos.y + constants::rel_to_core[d][i][1]
        );
        if (!rapid_zone(p)) return 2;
    }
    return 1;
}

bool check_still(Position pos, int d) {
    static Position p;
    for (int i = 0; i < 6; i++) {
        p = Position(
            pos.x + constants::rel_to_core[d][i][0],
            pos.y + constants::rel_to_core[d][i][1]
        );
        if (outside_map(p)) return false;
        if (!boat_zone(p)) return false;
    }
    return true;
}

bool check_ship(Position pos, int d, int flag = 1) { // flag: 1-transform, -1-inverse-transform
    static Position p;
    pos.x += constants::mv[d][0] * flag;
    pos.y += constants::mv[d][1] * flag;
    for (int i = 0; i < 6; i++) {
        p = Position(
            pos.x + constants::rel_to_core[d][i][0],
            pos.y + constants::rel_to_core[d][i][1]
        );
        if (outside_map(p)) return false;
        if (!boat_zone(p)) return false;
    }
    return true;
}

bool check_rotate(Position pos, int d, int r, int flag = 1) { // flag: 1-transform, -1-inverse-transform
    static Position p; int nd;
    if (flag > 0) {
        pos.x += constants::rot[r][d][0];
        pos.y += constants::rot[r][d][1];
        nd = new_direction(d, r);
    } else {
        nd = d;
        for (int t = 1; t <= 3; t++) {
            pos.x += constants::rot[r][nd][0];
            pos.y += constants::rot[r][nd][1];
            nd = new_direction(nd, r);
        }
    }
    for (int i = 0; i < 6; i++) {
        p = Position(
            pos.x + constants::rel_to_core[nd][i][0],
            pos.y + constants::rel_to_core[nd][i][1]
        );
        if (outside_map(p)) return false;
        if (!boat_zone(p)) return false;
    }
    return true;
}

void go(Boat &boat) {
    static Position p;
    int berth_id = boat.target, min_dis = constants::inf, cs = -2, w, r, nd;
    if (check_ship(boat.pos, boat.d)) {
        p = Position(
            boat.pos.x + constants::mv[boat.d][0],
            boat.pos.y + constants::mv[boat.d][1]
        );
        w = time_cost(p, boat.d);
        // std::cerr << "< " << p.x << ' ' << p.y << ' ' << -1 << ' ' << boat.d << ' ' << berths[berth_id].water_dis[boat.d][p.x][p.y] << ' ' << w << std::endl;
        if (min_dis > berths[berth_id].water_dis[boat.d][p.x][p.y] + w) {
            min_dis = berths[berth_id].water_dis[boat.d][p.x][p.y] + w;
            cs = -1;
        }
    }
    for (r = 0; r < 2; r++) {
        if (!check_rotate(boat.pos, boat.d, r)) continue;
        p = Position(
            boat.pos.x + constants::rot[r][boat.d][0],
            boat.pos.y + constants::rot[r][boat.d][1]
        );
        nd = new_direction(boat.d, r);
        w = time_cost(p, nd);
        // std::cerr << "> " << p.x << ' ' << p.y << ' ' << r << ' ' << nd << ' ' << berths[berth_id].water_dis[nd][p.x][p.y] << ' ' << w << std::endl;
        if (min_dis > berths[berth_id].water_dis[nd][p.x][p.y] + w) {
            min_dis = berths[berth_id].water_dis[nd][p.x][p.y] + w;
            cs = r;
        }
    }
    if (cs == -1) {
        printf("ship %d\n", boat.id);
        fflush(stdout);
        boat.pos = Position(
            boat.pos.x + constants::mv[boat.d][0],
            boat.pos.y + constants::mv[boat.d][1]
        );
    } else
    if (cs >= 0) {
        printf("rot %d %d\n", boat.id, cs);
        fflush(stdout);
        boat.pos = Position(
            boat.pos.x + constants::rot[cs][boat.d][0],
            boat.pos.y + constants::rot[cs][boat.d][1]
        );
        boat.d = new_direction(boat.d, cs);
    }
}

void back(Boat &boat) {
    static Position p;
    int w, min_dis = constants::inf, cs = -2, r, nd;
    if (check_ship(boat.pos, boat.d)) {
        p = Position(
            boat.pos.x + constants::mv[boat.d][0],
            boat.pos.y + constants::mv[boat.d][1]
        );
        w = time_cost(p, boat.d);
        if (min_dis > dis_terminal[boat.d][p.x][p.y] + w) {
            min_dis = dis_terminal[boat.d][p.x][p.y] + w;
            cs = -1;
        }
    }
    for (r = 0; r < 2; r++) {
        if (!check_rotate(boat.pos, boat.d, r)) continue;
        p = Position(
            boat.pos.x + constants::rot[r][boat.d][0],
            boat.pos.y + constants::rot[r][boat.d][1]
        );
        nd = new_direction(boat.d, r);
        w = time_cost(p, nd);
        if (min_dis > dis_terminal[nd][p.x][p.y] + w) {
            min_dis = dis_terminal[nd][p.x][p.y] + w;
            cs = r;
        }
    }
    // std::cerr << "! " << cs << ' ' << min_dis << std::endl;
    if (cs == -1) {
        printf("ship %d\n", boat.id);
        fflush(stdout);
        boat.pos = Position(
            boat.pos.x + constants::mv[boat.d][0],
            boat.pos.y + constants::mv[boat.d][1]
        );
    } else
    if (cs >= 0) {
        printf("rot %d %d\n", boat.id, cs);
        fflush(stdout);
        boat.pos = Position(
            boat.pos.x + constants::rot[r][boat.d][0],
            boat.pos.y + constants::rot[r][boat.d][1]
        );
        boat.d = new_direction(boat.d, cs);
    }
}

#endif