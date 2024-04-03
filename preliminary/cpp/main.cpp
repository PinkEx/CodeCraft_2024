#include<bits/stdc++.h>
using namespace std;
// constant
#define n_be 10 // number of berths
#define n_r 10 // number of robots
#define n_bo 5 // number of boats
#define esti_n_g 3000 // number of goods(estimated)
#define n_f 15000 // number of frames
#define len_env 200 // len of the map
#define len_window 1000 // len of the goods' appearing window
#define len_berth 4 // len of a berth
#define interal_transition_time 500 // the time boat move from berth to berth
#define last_transfer_time 12700 // the time threshold after which the boat should go back late
#define max_ending_time 14998 // the time all boats should finish work

int inf = 0x3f3f3f3f; // infinity
int ninf = -1e7; // negative infinity

int mv[4][2] = {
	0, 1,
	0, -1,
	-1, 0,
	1, 0,
}; // moving shifts: R/L/U/D

// variable
int T, M;
double G; // candidate decaying rate
char env[len_env][len_env]; // map information, 2-dimensions

int boat_capacity; // capacity of a boat
int sum_value = 0; // some of value
int n_bl = 0; // number of blocks
int p2b[len_env][len_env]; // block index of each position
int p2i[len_env][len_env]; // good index of each position
int p2r[len_env][len_env]; // robot id of each position, -1 when not occupied

int dis2g[esti_n_g][len_env][len_env];
int dis2b[n_be][len_env][len_env];
int targeted[n_r]; // no target: -1, otherwise: target good id
int temp_target[n_r][4];

// definition about things
struct Position {
	int x;
	int y;
	Position() {}
    Position(int _x, int _y) {
        x = _x;
        y = _y;
    }
};

struct Berth{
	int id;
	Position pos;
	int ttime; // transport time
	int lspeed; // speed of loading goods
	int reserved; // -1: not reserverd; otherwise: id of the boat
	int occupied; // -1: not occupied; otherwise: id of the boat
	queue<int> goods_temp; // temporary goods on the berth
	// int dis[len_env][len_env];
};

struct Boat{
	int id;
	int pos; // target berth
	int cap; // total capacity
	int load; // current load
	int status; // 0: moving, 1: loading or at virtual point, 2: wating outside a berth
	int inter_times; // times of inter transition
};

struct Good{
	int id;
	Position pos;
	int value;
	int itime; // time of appearance
	bool taken;
	// int dis[len_env][len_env];
};

struct Robot{
	int id;
	Position pos;
	bool running; // judge if the robot was crashed
	int good_taken; // no good: -1; else: good's id
	int last_dir; // the last move of the robot
	vector<int> target_goods;
	vector<int> target_berths;
};

// variable
Berth berths[n_be]; // berth instance list
Robot robots[n_r]; // robot instance list
Boat boats[n_bo]; // boat instance list
vector<Good> goods; // good instance list

// some useful utils

int func_outside_map(int x, int y){
	return x < 0 || x >= len_env || y < 0 || y >= len_env; 
}

// judge which berth the pos belongs to
int func_p2be(Position pos){
	int i;
	for(i = 0; i < n_be; i++){
		if(pos.x - berths[i].pos.x >= 0 && pos.x - berths[i].pos.x < len_berth
		&& pos.y - berths[i].pos.y >= 0 && pos.y - berths[i].pos.y < len_berth){
			return berths[i].id;
		}
	}
	return -1;
}

/*
	purpose: to decide which direction would be better
	ori: the original position of the robot,
	pos: the changed position of the robot,
	target_goods: the robot's target goods,
	r_id: the robots's id
*/
double func_eval_to_good(Position ori, Position pos, vector<int> target_goods, int r_id, int direction){
	priority_queue<double> scores;
	int index = 0;
	int i;
	double max_t = 0;
	double score = 0;
	double t[3] = {0};
	temp_target[r_id][direction] = -1;
	for(int g: target_goods){
		if(M == 2){
			for(i = 0; i < n_r; i++){
				if(i != r_id && targeted[i] == g) break;
			}
			if(i < r_id) continue;
		}
		double temp = 1.0 / (dis2g[g][pos.x][pos.y] + 0.5) - 1.0 / (dis2g[g][ori.x][ori.y] + 0.5);
		scores.push(temp);
		if(temp > max_t){
			index = g;
			max_t = temp;
		}
	}
	for(int i = 0; i < 3; i++){
		if (scores.empty()) break;
		t[i] = scores.top();
		scores.pop();
	}
	if(max_t > 0) temp_target[r_id][direction] = index;
	for(int i = 2; i >= 0; i--){
		score = score * G + max(t[i], 0.0);
	}
	return score;
}

/*
	purpose: to decide which direction would be better
	ori: the original position of the robot,
	pos: the changed position of the robot,
	target_births: the robot's target berths
*/
double func_eval_to_berth(Position ori, Position pos, vector<int> target_berths){
	priority_queue<double> scores;
	double score = 0;
	double t[3] = {0};
	for(auto b: target_berths){
		double coe = 1.0;
		if (T >= last_transfer_time) {
			if (berths[b].occupied == -1 && berths[b].reserved == -1) coe = 0;
		}
		double temp = 1.0 / (dis2b[b][pos.x][pos.y] + 0.5) - 1.0 / (dis2b[b][ori.x][ori.y] + 0.5);
		scores.push(temp * coe);
	}
	for(int i = 0; i < 3; i++){
		if (scores.empty()) break;
		t[i] = scores.top();
		scores.pop();
	}
	for(int i = 2; i >= 0; i--){
		score = score * G + max(t[i], 0.0);
	}
	return score;
}

// operations about berth
/*
	pos: the position of the berth,
	dis: the distance from the corresponding berth to (x, y)
*/

void get_berth_distance_matrix(Position pos, int dis[][len_env]){
	int i, dx, dy, direction;
	for(i = 0; i < len_env; i++)
		memset(dis[i], 63, sizeof dis[i]);
	queue<Position> q;
	for(dx = 0; dx < len_berth; dx++){
		for(dy = 0; dy < len_berth; dy++){
			dis[pos.x + dx][pos.y + dy] = 0;
			q.push(Position(pos.x + dx, pos.y + dy));
		}
	}
	while(!q.empty()){
		Position p = q.front();
		q.pop();
		for(direction = 0; direction < 4; direction++){
			int x = p.x + mv[direction][0];
			int y = p.y + mv[direction][1];
			if(func_outside_map(x, y)) continue;
			if(env[x][y] == '#' || env[x][y] == '*') continue;
			if(dis[x][y] >= inf){
				dis[x][y] = dis[p.x][p.y] + 1;
				q.push(Position(x, y));
			}
		}
	}
}

// load goods
void load(Berth &be){
	int i;
	for(i = 0; i < be.lspeed; i++){
		if(be.goods_temp.size() > 0 && boats[be.occupied].load < boats[be.occupied].cap){
			int g = be.goods_temp.front();
			be.goods_temp.pop();
			boats[be.occupied].load += 1;
		} else return;
	}
}

// operations about goods
int available(Good g, int frame){
	return frame - g.itime < len_window && !g.taken;
}

/*
	pos: the position of the berth,
	dis: the distance from the corresponding berth to (x, y)
*/
void get_goods_distance_matrix(Position pos, int dis[][len_env]){
	int i, direction;
	for(i = 0; i < len_env; i++)
		memset(dis[i], 63, sizeof dis[i]);
	queue<Position> q;
	q.push(pos);
	dis[pos.x][pos.y] = 0;
	while(!q.empty()){
		Position p = q.front();
		q.pop();
		for(direction = 0; direction < 4; direction++){
			int x = p.x + mv[direction][0];
			int y = p.y + mv[direction][1];
			if(func_outside_map(x, y)) continue;
			if(env[x][y] == '#' || env[x][y] == '*') continue;
			if(dis[x][y] >= inf){
				dis[x][y] = dis[p.x][p.y] + 1;
				q.push(Position(x, y));
			}
		}
	}
}

// operations about goods
// get goods
int get(Robot &r){
	if (!r.running) return -1; // robot not running
	if (r.good_taken != -1) return -1; // robot already with good taken
	if (p2i[r.pos.x][r.pos.y] == -1) return -1; // no good to take
	r.good_taken = p2i[r.pos.x][r.pos.y];
	r.last_dir = -1;
	return r.good_taken;
}

// pull goods
void pull(Robot &r, int &good_id, int &berth_id){
	// robot not running
	if (!r.running) { 
		good_id = -1;
		berth_id = -1;
		return;
	}
	// robot with no good taken
	if (r.good_taken == -1) {
		good_id = -1;
		berth_id = -1;
		return;
	}
	// not on a berth
	if (env[r.pos.x][r.pos.y] != 'B') {
		good_id = -1;
		berth_id = -1;
		return;
	}
	good_id = r.good_taken;
	berth_id = func_p2be(r.pos);
	r.good_taken = -1;
	r.last_dir = -1;
}

// compute the score of each direction to choose a better one
double move(const Robot &r, int direction){
	if (!r.running) return ninf; // robot not running
	int x = r.pos.x + mv[direction][0];
	int y = r.pos.y + mv[direction][1];
	if (func_outside_map(x, y)) return ninf; // outside the map
	if (env[x][y] == '#' || env[x][y] == '*') return ninf; // not a land or a berth
	if (p2r[x][y] >= 0) return ninf; // occupied by other robots
	
	double score = 0;
	if (r.good_taken != -1){
		score = func_eval_to_berth(r.pos, Position(x, y), r.target_berths);
	} else {
		score = func_eval_to_good(r.pos, Position(x, y), r.target_goods, r.id, direction);
	}
	if (r.last_dir == direction) score *= 1.001; // encourage to keep diretcion
	else if ((r.last_dir ^ direction) == 1) score *= 0.001; // discourage to turn back
	return score;
}	

// divide connecting areas into different blocks
void search_block(int x, int y, int bindex){
	p2b[x][y] = bindex;
	queue<Position> q;
	q.push(Position(x, y));
	while(!q.empty()){
		Position p = q.front();
		q.pop();
		for(int direction = 0; direction < 4; direction++){
			x = p.x + mv[direction][0];
			y = p.y + mv[direction][1];
			if (func_outside_map(x, y)) continue; //outside the map
			if (env[x][y] == '#' || env[x][y] == '*') continue; 
			if (p2b[x][y] == 0){
				p2b[x][y] = bindex;
				q.push(Position(x, y));
			}
		}
	}
}

void preprocess(){
	for(int i = 0; i < len_env; i++){
		for(int j = 0; j < len_env; j++){
			if (env[i][j] == '#' || env[i][j] == '*'){
				p2b[i][j] = -1;
				continue;
			}
			if (p2b[i][j] == 0){
				n_bl += 1;
				search_block(i, j, n_bl);
			}
		} 
	}
}

// initialization 
void Init()
{
	memset(p2b, 0, sizeof p2b);
	memset(p2i, -1, sizeof p2i);
	memset(p2r, -1, sizeof p2r);
	memset(targeted, -1, sizeof targeted);
	// input map
    for(int i = 0; i < len_env; i++){
    	scanf("%s", &env[i]);
	}
	if (env[74][74] == 'B') {
		M = 1;
		G = 0.3;
	} else if (env[74][74] == '#') {
		M = 2;
		G = 0.942;
	} else {
		M = 3;
		G = 0.355;
	}
	// input information of the berth
    for(int i = 0; i < n_be; i++)
    {
        int id;
        scanf("%d", &id);
        berths[id].id = id;
        scanf("%d%d%d%d", &berths[id].pos.x, &berths[id].pos.y, &berths[id].ttime, &berths[id].lspeed);
		berths[id].reserved = -1;
		berths[id].occupied = -1;
    	get_berth_distance_matrix(berths[id].pos, dis2b[id]);
		// cerr << i << " " << berths[id].ttime << endl;
	}
	for(int i = 0; i < n_r; i++){
		robots[i].id = i;
		robots[i].running = true;
		robots[i].good_taken = -1;
		robots[i].last_dir = -1;
	}
    scanf("%d", &boat_capacity);
	// cerr << "{\"boat_capacity\":" << boat_capacity << "}" << endl;
	for(int i = 0; i < n_bo; i++){
		boats[i].id = i;
		boats[i].cap = boat_capacity;
		boats[i].pos = -1;
		boats[i].status = 1;
		boats[i].inter_times = 0;
	}
	preprocess();
    char okk[10];
    scanf("%s", okk);
    puts("OK");
    fflush(stdout);
}

int Input()
{
	int frame_id, money;
    scanf("%d%d", &frame_id, &money);
	// fstream log_file("log.txt", ios::app);
	// log_file << frame_id << " " << money << endl;
	// log_file.close();
    // judging if goods had disappeared
	for(auto g: goods){
		if (!available(g, frame_id) and p2i[g.pos.x][g.pos.y] == g.id)
			p2i[g.pos.x][g.pos.y] = -1;
	}
	// Goods Information
    int num; double val_threshold = (M == 1 ? 80: M == 2 ? 60: 120);
    scanf("%d", &num);
    for(int i = 0; i < num; i++){
        int x, y, val;
        Good temp;
		scanf("%d%d%d", &x, &y, &val);
		if (val < val_threshold) continue; // value too low: A HYPERPARAMETER TO SWITCH
        temp.id = goods.size();
        temp.pos.x = x;
        temp.pos.y = y;
        temp.value = val;
        temp.itime = frame_id;
		temp.taken = false;
        p2i[x][y] = temp.id;
        get_goods_distance_matrix(temp.pos, dis2g[temp.id]);
        goods.push_back(temp);
    }
	// cerr << frame_id << " !!" << endl;
	// Robots Information
    for(int i = 0; i < n_r; i++){
		int good_taken, x, y, status;
        scanf("%d%d%d%d", &good_taken, &x, &y, &status);
		robots[i].pos = Position(x, y);
        if (!good_taken) robots[i].good_taken = -1;
        robots[i].running = (status == 1);
        if (robots[i].running) {
        	if (robots[i].good_taken == -1){
        		robots[i].target_goods.clear();
				for(Good g: goods) {
					if (available(g, frame_id) && (dis2g[g.id][x][y] <= 200 || dis2g[g.id][x][y] <= inf && robots[i].target_goods.size() == 0)) // distance too large: A HYPERPARAMETER TO SWITCH
						robots[i].target_goods.push_back(g.id);
				}
			} else {
				robots[i].target_berths.clear();
				for(int j = 0; j < n_be; j++){
					if (p2b[berths[j].pos.x][berths[j].pos.y] != p2b[x][y]) continue;
					robots[i].target_berths.push_back(berths[j].id);
				}
			}
		}
    }
	// cerr << frame_id << " !!!" << endl;
    // Boats Informatipn
    for(int i = 0; i < n_bo; i ++)
        scanf("%d%d", &boats[i].status, &boats[i].pos);
    char okk[10];
    scanf("%s", okk);
    return frame_id;
}

// some utils to solve the problem
void robot_dispatch(int frame_id){
	int i, good_id, berth_id;
	for(i = 0; i < n_r; i++){
		p2r[robots[i].pos.x][robots[i].pos.y] = i;
	}
	for(i = 0; i < n_r; i++){
		if (frame_id < i) continue; // async start
		good_id = get(robots[i]);
		if (good_id != -1){
			goods[good_id].taken = true;
			printf("get %d\n", i);
			fflush(stdout);
			// cerr << "GET!" << frame_id << " " << i << " " << good_id << endl;
			continue;
		}
		pull(robots[i], good_id, berth_id);
		if (good_id != -1){
			berths[berth_id].goods_temp.push(good_id);
			printf("pull %d\n", i);
			fflush(stdout);
			// cerr << "PULL!" << frame_id << " " << i << " " << good_id << " " << berth_id << endl;
			sum_value += goods[good_id].value;
			continue;
		}
		double max_ev = 0;
		int d = -1;
		for(int j = 0; j < 4; j++){
			double esti_value = move(robots[i], j);
			if(max_ev < esti_value){
				max_ev = esti_value;
				d = j;
				if(robots[i].good_taken != -1) targeted[i] = -1;
				else targeted[i] = temp_target[i][d];
			}
		}
		if (max_ev > 0){
			robots[i].last_dir = d;
			p2r[robots[i].pos.x][robots[i].pos.y] = -1;
			p2r[robots[i].pos.x + mv[d][0]][robots[i].pos.y + mv[d][1]] = i;
			printf("move %d %d\n", i, d);
			fflush(stdout);
		}
	}
}
// just for following sort
struct to_order{
	int id;
	int sum;	
};
int cmp(to_order x, to_order y){
	return x.sum > y.sum;
}
void boat_dispatch(int frame_id){
	int i, j, k;
	bool flag;
	to_order ordered_berths[n_be];
	for(i = 0; i < n_be; i++){
		int sum = 0, count = berths[i].goods_temp.size();
		for(j = 0; j < count; j++){
			int g_id = berths[i].goods_temp.front();
			berths[i].goods_temp.pop();
			berths[i].goods_temp.push(g_id);
			sum += goods[g_id].value;
		}
		ordered_berths[i].id = berths[i].id;
		ordered_berths[i].sum = sum;
	}
	sort(ordered_berths, ordered_berths + n_be, cmp);
	for(i = 0; i < n_bo; i++){
		if (boats[i].pos == -1){ // transfer the goods back
			boats[i].load = 0;
			boats[i].inter_times = 0;
			continue;
		}
		if (boats[i].status == 0) continue; // on transfering
		if (boats[i].status == 1) berths[boats[i].pos].occupied = i;
		if (boats[i].load == boats[i].cap
		|| berths[boats[i].pos].goods_temp.size() == 0
		|| frame_id + berths[boats[i].pos].ttime > max_ending_time) {
			double load_threshold = (M == 2 ? 0.96: 0.8);
			if (boats[i].status == 1 && boats[i].load < boats[i].cap * load_threshold && ordered_berths[0].sum > 0) { // optimal load rate: A HYPERPARAMETER TO SWITCH
				flag = false;
				for(k = 0; k < n_be; k++){
					j = ordered_berths[k].id;
					if (j == boats[i].pos) continue;
					if (berths[j].reserved != -1 || berths[j].occupied != -1) continue;
					if (frame_id + interal_transition_time + berths[j].ttime + boat_capacity >= max_ending_time) continue;
					flag = true;
					// for (int _ = 0; _ < boats[i].inter_times; ++_) cerr << '*';
					// cerr << "boat " << i << ", berth " << boats[i].pos << " to berth " << j << ",  @ frame " << frame_id << ": " << boats[i].load << "/" << boats[i].cap << endl;
					berths[boats[i].pos].occupied = -1;
					berths[boats[i].pos].reserved = -1;
					berths[j].reserved = i;
					boats[i].pos = j;
					boats[i].inter_times += 1;
					printf("ship %d %d\n", i, j);
					fflush(stdout);
					break;
				}
				if (flag) continue;
			}
			if (frame_id < last_transfer_time ||
			frame_id >= last_transfer_time && (frame_id + berths[boats[i].pos].ttime >= max_ending_time || boats[i].load == boats[i].cap)) {
				// for (int _ = 0; _ < boats[i].inter_times; ++_) cerr << '*';
				// cerr << "boat " << i << ", berth " << boats[i].pos << " to berth -1,  @ frame " << frame_id << ": " << boats[i].load << "/" << boats[i].cap << ", #inter-trans = " << boats[i].inter_times << endl;
				berths[boats[i].pos].occupied = -1;
				berths[boats[i].pos].reserved = -1;
				boats[i].status = 0;
				boats[i].pos = -1;
				printf("go %d\n", i);
				fflush(stdout);
			}
		}
	}
	for(int k = 0; k < n_be; k++){
		i = ordered_berths[k].id;
		if (berths[i].reserved != -1 || berths[i].occupied != -1) continue;
		if (frame_id + berths[i].ttime * 2 + boat_capacity >= max_ending_time) continue;
		for(j = 0; j < n_bo; j++){
			if (boats[j].status == 1 && boats[j].pos == -1){
				// cerr << "boat " << j << ", berth " << boats[j].pos << " to berth " << i << ",  @ frame " << frame_id << ": " << boats[j].load << "/" << boats[j].cap << endl;
				berths[i].reserved = j;
				boats[j].pos = i;
				printf("ship %d %d\n", j, i);
				fflush(stdout);
				break;
			}
		}
	}
	for(i = 0; i < n_be; i++){
		if (berths[i].occupied == -1) continue;
		load(berths[i]);
	}
}

void solve_frame(int frame_id){
	robot_dispatch(frame_id);
	boat_dispatch(frame_id);
	// int i, j;
	// cerr << "[frame " << frame_id << "]" << endl;	
	// for(i = 0; i < n_be; i++){
	// 	cerr << "  berth " << i << ": ";
	// 	int count = berths[i].goods_temp.size();
	// 	for(j = 0; j < count; j++){
	// 		int g_id = berths[i].goods_temp.front();
	// 		berths[i].goods_temp.pop();
	// 		berths[i].goods_temp.push(g_id);
	// 		cerr << goods[g_id].value;
	// 		if (j < count - 1) cerr << ", ";
	// 	}
	// 	cerr << endl;
	// }
	puts("OK");
	fflush(stdout);
}

int main(){
    Init();
    for(int frame = 1; frame <= 15000; frame++)
    {
		clock_t t0 = clock();
        T = Input();
		clock_t t1 = clock();
		solve_frame(frame);
		clock_t t2 = clock();
		// if (frame == 14999) {
			// cerr << "{\"sum_value\":" << sum_value << "}" << endl;
		// }
    }
	return 0;
}
