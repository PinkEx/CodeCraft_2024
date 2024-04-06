#include <bits/stdc++.h>
#include "constants.h"
#include "variables.h"
#include "functions.h"

void get_distance_from_berth(int berth_id) {    
	int i, x, y, w, d, nd, r, t; Position p;
	std::queue < Position > q;
	std::queue < std::pair < int, Position > > q_d;
    bool in_queue[constants::len_env][constants::len_env];
    bool in_queue_d[4][constants::len_env][constants::len_env];
    // get_whole_berth
    q.push(berths[berth_id].pos);
    which_berth[berths[berth_id].pos.x][berths[berth_id].pos.y] = berth_id;
	while (!q.empty()) {
		p = q.front(), q.pop();
		for (d = 0; d < 4; d++) {
			x = p.x + constants::mv[d][0], y = p.y + constants::mv[d][1];
			if (outside_map(Position(x, y))) continue;
            if (which_berth[x][y] == -1 && berth_or_dock_point(Position(x, y))) {
                q.push(Position(x, y));
                which_berth[x][y] = berth_id;
            }
		}
	}
    // get_land_distance
    memset(berths[berth_id].land_dis, 0x3f, sizeof berths[berth_id].land_dis);
    berths[berth_id].land_dis[berths[berth_id].pos.x][berths[berth_id].pos.y] = 0;
    memset(in_queue, false, sizeof in_queue);
    q.push(berths[berth_id].pos);
    while (!q.empty()) {
		p = q.front(), q.pop(), in_queue[p.x][p.y] = false;
		for (d = 0; d < 4; d++) {
			x = p.x + constants::mv[d][0], y = p.y + constants::mv[d][1];
			if (outside_map(Position(x, y))) continue;
            if (!robot_zone(Position(x, y))) continue;
            if (env[x][y] == 'B' && berths[berth_id].land_dis[x][y] > 0 && which_berth[x][y] == berth_id) {
                berths[berth_id].land_dis[x][y] = 0;
                if (!in_queue[x][y]) {
                    q.push(Position(x, y));
                    in_queue[x][y] = true;
                }
                continue;
            }
            if (berths[berth_id].land_dis[x][y] > berths[berth_id].land_dis[p.x][p.y] + 1) {
                berths[berth_id].land_dis[x][y] = berths[berth_id].land_dis[p.x][p.y] + 1;
                if (!in_queue[x][y]) {
                    q.push(Position(x, y));
                    in_queue[x][y] = true;
                }
            }
		}
    }
    // get_water_distance
    memset(berths[berth_id].water_dis, 0x3f, sizeof berths[berth_id].water_dis);
    memset(in_queue_d, false, sizeof in_queue_d);
    for (x = 0; x < constants::len_env; x++) {
        for (y = 0; y < constants::len_env; y++) {
            if (which_berth[x][y] != berth_id) continue;
            for (d = 0; d < 4; d++) {
                if (!check_still(Position(x, y), d)) continue;
                berths[berth_id].water_dis[d][x][y] = 0;
                in_queue_d[d][x][y] = true;
                q_d.push(std::make_pair(d, Position(x, y)));
            }
        }
    }
    while (!q_d.empty()) {
		d = q_d.front().first, p = q_d.front().second;
        q_d.pop(), in_queue_d[d][p.x][p.y] = false;
        w = time_cost(p, d);
        // check SHIP
        if (check_ship(p, d, -1) > 0) {
            x = p.x - constants::mv[d][0], y = p.y - constants::mv[d][1];
            if (berths[berth_id].water_dis[d][x][y] > berths[berth_id].water_dis[d][p.x][p.y] + w) {
                berths[berth_id].water_dis[d][x][y] = berths[berth_id].water_dis[d][p.x][p.y] + w;
                if (!in_queue_d[d][x][y]) {
                    q_d.push(std::make_pair(d, Position(x, y)));
                    in_queue_d[d][x][y] = true;
                }
            }
        }
        // check ROT
        for (r = 0; r < 2; r++) {
            if (check_rotate(p, d, r, -1) > 0) {
                x = p.x, y = p.y, nd = d;
                for (t = 1; t <= 3; t++) {
                    x += constants::rot[r][nd][0];
                    y += constants::rot[r][nd][1];
                    nd = new_direction(nd, r);
                }
                if (berths[berth_id].water_dis[nd][x][y] > berths[berth_id].water_dis[d][p.x][p.y] + w) {
                    berths[berth_id].water_dis[nd][x][y] = berths[berth_id].water_dis[d][p.x][p.y] + w;
                    if (!in_queue_d[nd][x][y]) {
                        q_d.push(std::make_pair(nd, Position(x, y)));
                        in_queue_d[nd][x][y] = true;
                    }
                }
            }
        }
    }
}
void get_distance_from_terminal() {
    int d, x, y, w, r, nd, t; Position p;
    bool in_queue_d[4][constants::len_env][constants::len_env];
	std::queue < std::pair < int, Position > > q_d;
    memset(dis_terminal, 0x3f, sizeof dis_terminal);
    memset(in_queue_d, false, sizeof in_queue_d);
    for (auto &terminal: terminals) {
        for (d = 0; d < 4; d++) {
            if (!check_still(terminal, d)) continue;
            dis_terminal[d][terminal.x][terminal.y] = 0;
            in_queue_d[d][terminal.x][terminal.y] = true;
            q_d.push(std::make_pair(d, Position(terminal.x, terminal.y)));
        }
    }
    while (!q_d.empty()) {
		d = q_d.front().first, p = q_d.front().second;
        q_d.pop(), in_queue_d[d][p.x][p.y] = false;
        w = time_cost(p, d);
        // check SHIP
        if (check_ship(p, d, -1) > 0) {
            x = p.x - constants::mv[d][0], y = p.y - constants::mv[d][1];
            if (dis_terminal[d][x][y] > dis_terminal[d][p.x][p.y] + w) {
                dis_terminal[d][x][y] = dis_terminal[d][p.x][p.y] + w;
                if (!in_queue_d[d][x][y]) {
                    q_d.push(std::make_pair(d, Position(x, y)));
                    in_queue_d[d][x][y] = true;
                }
            }
        }
        // check ROT
        for (r = 0; r < 2; r++) {
            if (check_rotate(p, d, r, -1) > 0) {
                x = p.x, y = p.y, nd = d;
                for (t = 1; t <= 3; t++) {
                    x += constants::rot[r][nd][0];
                    y += constants::rot[r][nd][1];
                    nd = new_direction(nd, r);
                }
                if (dis_terminal[nd][x][y] > dis_terminal[d][p.x][p.y] + w) {
                    dis_terminal[nd][x][y] = dis_terminal[d][p.x][p.y] + w;
                    if (!in_queue_d[nd][x][y]) {
                        q_d.push(std::make_pair(nd, Position(x, y)));
                        in_queue_d[nd][x][y] = true;
                    }
                }
            }
        }
    }
}
void preprocess() {
    for (auto &berth: berths) {
        get_distance_from_berth(berth.id);
    }
    get_distance_from_terminal();
}
void init() {
    memset(which_berth, -1, sizeof which_berth);
    memset(which_boat, -1, sizeof which_boat);
    memset(which_good, -1, sizeof which_good);
    memset(which_robot, -1, sizeof which_robot);
    // input starts
    robot_spawn_points.clear();
    for (int i = 0; i < constants::len_env; i++) {
        scanf("%s", &env[i]);
        for (int j = 0; j < constants::len_env; j++) {
            if (robot_spawn_point(Position(i, j))) {
                robot_spawn_points.emplace_back(Position(i, j));
            } else
            if (boat_spawn_point(Position(i, j))) {
                boat_spawn_points.emplace_back(Position(i, j));
            } else
            if (env[i][j] == 'T') {
                terminals.emplace_back(Position(i, j));
            }
        }
    }
    int num_berth, id, x, y, velocity;
    berths.clear();
    scanf("%d", &num_berth);
    for (int i = 0; i < num_berth; i++) {
        scanf("%d%d%d%d", &id, &x, &y, &velocity);
        berths.emplace_back(Berth(id, Position(x, y), velocity));
    }
    scanf("%d", &boat_capacity);
    load_threshold = int(boat_capacity);
    // input ends
	preprocess();
    confirm_ok(true, true);
}
void get_distance_from_good(int good_id) {
	int i, x, y, w, d; Position p;
    bool in_queue[constants::len_env][constants::len_env];
	std::queue <Position> q;
	memset(goods[good_id].dis, 0x3f, sizeof goods[good_id].dis);
    goods[good_id].dis[goods[good_id].pos.x][goods[good_id].pos.y] = 0;
    memset(in_queue, false, sizeof in_queue);
    q.push(goods[good_id].pos);
	while (!q.empty()) {
		p = q.front(), q.pop(), in_queue[p.x][p.y] = false;
		for (d = 0; d < 4; d++) {
			x = p.x + constants::mv[d][0], y = p.y + constants::mv[d][1];
			if (outside_map(Position(x, y))) continue;
            if (!robot_zone(Position(x, y))) continue;
			if (goods[good_id].dis[x][y] > goods[good_id].dis[p.x][p.y] + 1) {
                goods[good_id].dis[x][y] = goods[good_id].dis[p.x][p.y] + 1;
                if (!in_queue[x][y]) {
    				q.push(Position(x, y));
                    in_queue[x][y] = true;
                }
			}
		}
	}
}

void input_frame(int frame_id) {
    int k, r, b, x, y, v, id, status, num_good, d, fd;
    scanf("%d", &money);
    scanf("%d", &k);
    for (int i = 0; i < k; i++) {
        scanf("%d%d%d", &x, &y, &v);
        if (v == 0) {
            if (which_good[x][y] == -1) continue;
            goods[which_good[x][y]].available = false;
            which_good[x][y] = -1;
        } else {
            int id = goods.size();
            if (v < 40) continue;
            goods.emplace_back(Good(id, Position(x, y), v));
            which_good[x][y] = id;
            get_distance_from_good(id);
        }
    }
    scanf("%d", &r);
    for (int i = 0; i < r; i++) {
        scanf("%d%d%d%d", &id, &status, &x, &y);
        robots[id].pos = Position(x, y);
        if (!status) robots[id].good_taken = -1;
    }
    for (auto &robot: robots) {
        x = robot.pos.x, y = robot.pos.y;
        if (robot.good_taken == -1) {
            if (~robot.matched_good && !goods[robot.matched_good].available) robot.matched_good = -1;
            robot.target_goods.clear();
            for (auto &good: goods) {
                if (good.available && good.dis[x][y] <= constants::inf
                    && (good.pursuer == -1 || good.pursuer == robot.id)) {
                    robot.target_goods.emplace_back(good.id);
                }
            }
            if (robot.target_goods.empty()) continue;
            sort(robot.target_goods.begin(), robot.target_goods.end(), [x, y](int e, int f) {
                return goods[e].value / pow(1.0 + goods[e].dis[x][y], 0.3) > goods[f].value / pow(1.0 + goods[f].dis[x][y], 0.3);
            });
            int closest = *robot.target_goods.begin();
            if (~robot.matched_good && closest != robot.matched_good) goods[robot.matched_good].pursuer = -1;
            goods[closest].pursuer = robot.id;
            robot.matched_good = closest;
        } else {
            robot.target_berths.clear();
            for (auto &berth: berths) {
                if (berth.land_dis[x][y] <= constants::inf) {
                    robot.target_berths.emplace_back(berth.id);
                }
            }
            sort(robot.target_berths.begin(), robot.target_berths.end(), [x, y](int e, int f) {
                return berths[e].land_dis[x][y] < berths[f].land_dis[x][y];
            });
        }
    }
    scanf("%d", &b);
    for (int i = 0; i < b; i++) {
        scanf("%d%d%d%d%d%d", &id, &num_good, &x, &y, &d, &status);
        boats[id].pos = Position(x, y);
        boats[id].d = d;
        boats[id].status = status;
    }
    for (auto &boat: boats) {
        x = boat.pos.x, y = boat.pos.y, d = boat.d;
        if (~boat.target) continue;
        if (!boat.enough_load(load_threshold)) {
            int max_num = 0;
            for (auto &berth: berths) {
                if (berth.received_goods.empty()) continue;
                if (~berth.reserved) continue;
                if (max_num < berth.received_goods.size() && berth.water_dis[d][x][y] < constants::inf) {
                    max_num = berth.received_goods.size();
                    boat.target = berth.id;
                }
            }
            if (~boat.target) berths[boat.target].reserved = boat.id;
        } else boat.target = -2;
    }
    confirm_ok(true, false);
}
void robot_dispatch(int frame_id) {
    int good_id, berth_id;
    for (auto &robot: robots) {
        which_robot[robot.pos.x][robot.pos.y] = robot.id;
    }
    for (auto &robot: robots) {
        good_id = get(robot);
        if (good_id != -1) {
            goods[good_id].available = false;
            printf("get %d\n", robot.id);
            fflush(stdout);
			continue;
        }
        pull(robot, good_id, berth_id);
		if (good_id != -1) {
            sum_value += goods[good_id].value;
            berths[berth_id].received_goods.push(good_id);
            // std::cerr << "+ " << goods[good_id].value << std::endl;
			printf("pull %d\n", robot.id);
			fflush(stdout);
			continue;
		}
		double max_ev = 0;
		int cs = -1;
		for (int d = 0; d < 4; d++) {
			double esti_value = move(robot, d);
			if (max_ev < esti_value) {
				max_ev = esti_value;
				cs = d;
			}
		}
		if (max_ev > 0) {
            if (robot.last_dir != cs) {
                robot.last_time = 0;
            }
			robot.last_dir = cs;
            robot.last_time++;
			which_robot[robot.pos.x][robot.pos.y] = -1;
			which_robot[robot.pos.x + constants::mv[cs][0]][robot.pos.y + constants::mv[cs][1]] = robot.id;
			printf("move %d %d\n", robot.id, cs);
			fflush(stdout);
		} else {
            robot.last_dir = -1;
            robot.last_time = 1;
        }
	}
}
void boat_dispatch(int frame_id) {
    int berth_id, boat_id, x, y;
    for (x = 0; x < constants::len_env; x++) {
        for (y = 0; y < constants::len_env; y++) {
            which_boat[x][y] = -1;
        }
    }
    for (auto &boat: boats) {
        arrive(boat);
    }
    for (auto &boat: boats) {
        berth_id = which_berth[boat.pos.x][boat.pos.y];
        if (boat.status == 1) continue;
        if (boat.status == 2) {
            if (!boat.enough_load(load_threshold) && !berths[berth_id].received_goods.empty()) {
                load(berths[berth_id]);
            } else {
                depart(boat);
                berths[berth_id].reserved = -1;
                berths[berth_id].occupied = -1;
                printf("dept %d\n", boat.id);
                fflush(stdout);
            }
            continue;
        }
        if (berth_id != -1 && boat.target == berth_id && berths[berth_id].occupied == -1) {
            depart(boat);
            berths[berth_id].occupied = boat.id;
            boat.target = -1;
            printf("berth %d\n", boat.id);
            fflush(stdout);
            continue;
        }
        // std::cerr << '@' << frame_id << ' ' << boat.id << ": " << boat.pos.x << ' ' << boat.pos.y << ' ' << boat.d << ' ' << boat.load << std::endl;
        // std::cerr << boat.status << ' ' << boat.target << ' ' << berth_id << std::endl;
        if (boat.target >= 0) {
            depart(boat);
            go(boat);
            arrive(boat);
        } else if (boat.target == -2) {
            if (dis_terminal[boat.d][boat.pos.x][boat.pos.y] == 0) {
                boat.target = -1;
                boat.load = 0;
            } else {
                depart(boat);
                back(boat);
                arrive(boat);
            }
        }
    }
}
void labour_purchase(int frame_id) {
    if (frame_id - last_purchase_robot > 20 && money >= constants::robot_cost && robots.size() < 16) {
        last_purchase_robot = frame_id;
        std::random_shuffle(robot_spawn_points.begin(), robot_spawn_points.end());
        for (auto pos: robot_spawn_points) {
            if (true) {
                lbot(pos);
                break;
            }
        }
    }
    if (frame_id - last_purchase_boat > 40 && money >= constants::boat_cost && boats.size() < 1) {
        last_purchase_boat = frame_id;
        std::random_shuffle(boat_spawn_points.begin(), boat_spawn_points.end());
        for (auto pos: boat_spawn_points) {
            if (true) {
                lboat(pos);
                break;
            }
        }
    }
}
void solve_frame(int frame_id) {
    robot_dispatch(frame_id);
    boat_dispatch(frame_id);
    labour_purchase(frame_id);
    confirm_ok(false, true);
}

int main() {
    int frame_id;
    init();
    while (scanf("%d", &frame_id) != EOF) {
        input_frame(frame_id);
        solve_frame(frame_id);
        // if (frame_id == 14999) {
            // for (auto berth: berths) {
            //     std::cerr << berth.id << std::endl;
            //     while (!berth.received_goods.empty()) {
            //         std::cerr << "| " << goods[berth.received_goods.front()].value << std::endl;
            //         berth.received_goods.pop();
            //     }
            // }
            // std::cerr << "sum_value = " << sum_value << std::endl;
        // }
    }
    return 0;
}