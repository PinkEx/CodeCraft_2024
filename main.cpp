#include<bits/stdc++.h>
using namespace std;
//constant
#define n_be 10 // number of berths
#define n_r 10 // number of robots
#define n_bo 5 // number of boats
#define n_f 15000 // number of frames
#define len_env 200 // len of the map
#define len_window 1000 // len of the goods' appearing window
#define len_berth 4 // len of a berth

#define gamma 0.2 // candidate decaying rate
int inf = 1e7; // infinity
int ninf = -1e7; // negative infinity

int mv[4][2] = {
	0, 1,
	0, -1,
	-1, 0,
	1, 0,
};// moving shifts: R/L/U/D

// variable

char env[len_env][len_env]; // map information, 2-dimensions

int n_bl = 0; // number of blocks
int p2b[len_env][len_env]; // block index of each position
int p2i[len_env][len_env]; // good index of each position
int p2r[len_env][len_env]; // robot id of each position, -1 when not occupied

// definition about things
struct Position{
	int x;
	int y;
	Position() {}
    Position(int startX, int startY) {
        x = startX;
        y = startY;
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
	int dis[len_env][len_env];
};

struct Boat{
	int id;
	int pos; // target berth
	int cap; // total capacity
	int load; // current load
	int status; // 0: moving, 1: loading or at virtual point, 2: wating outside a berth
};

struct Good{
	int id;
	Position pos;
	int value;
	int itime; // time of appearance
	int dis[len_env][len_env];
	bool taken;
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
	for(i = 0; i < len_berth; i++){
		if(pos.x - berths[i].pos.x >= 0 && pos.x - berths[i].pos.x < len_berth && pos.y - berths[i].pos.y >= 0 && pos.y - berths[i].pos.y < len_berth){
			return berths[i].id;
		}
	}
}

/*
	purpose: to decide which direction would be better
	ori: the original position of the robot,
	pos: the changed position of the robot,
	target_goods: the robot's target goods
*/
double func_eval_to_good(Position ori, Position pos, vector<int> target_goods){
	priority_queue<double> scores;
	double score = 0;
	double t[3] = {0};
	for(vector<int>::iterator it = target_goods.begin(); it < target_goods.end(); it++){
		double temp = 1.0 / (goods[*it].dis[pos.x][pos.y] + 0.5) - 1.0 / (goods[*it].dis[ori.x][ori.y] + 0.5);
		scores.push(temp);
	}
	for(int i = 0; i < 3; i++){
		double s = scores.top();
		scores.pop();
		t[i] = s;
	}
	for(int i = 2; i >= 0; i--){
		double x = t[i];
		if (x < 0) x = 0;
		score = score * gamma + x;
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
	for(vector<int>::iterator it = target_berths.begin(); it < target_berths.end(); it++){
		double temp = 1.0 / (berths[*it].dis[pos.x][pos.y] + 0.5) - 1.0 / (berths[*it].dis[ori.x][ori.y] + 0.5);
		scores.push(temp);
	}
	for(int i = 0; i < 3; i++){
		double s = scores.top();
		scores.pop();
		t[i] = s;
	}
	for(int i = 2; i >= 0; i--){
		double x = t[i];
		if(x < 0) x = 0;
		score = score * gamma + x;
	}
	return score;
}

// operations about berth
/*
	pos: the position of the berth,
	dis: the distance from the corresponding berth to (x, y)
*/

void get_berth_distance_matrix(Position pos, int dis[][len_env]){
	int dx, dy, direction;
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
void load(Berth& be){
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
// operations about goods
/*
	pos: the position of the berth,
	dis: the distance from the corresponding berth to (x, y)
*/

void get_goods_distance_matrix(Position pos, int dis[][len_env]){
	int dx, dy, direction;
	queue<Position> q;
	q.push(Position(pos.x, pos.y));
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
int pull(Robot &r, int* good_id, int* berth_id ){
	// robot not running
	if (!r.running) { 
		*good_id = -1;
		*berth_id = -1;
	}
	// robot already with good taken
	if (r.good_taken != -1) {
		*good_id = -1;
		*berth_id = -1;
	} 
	// not on a berth
	if (env[r.pos.x][r.pos.y] != 'B') {
		*good_id = -1;
		*berth_id = -1;
	}
	*good_id = r.good_taken;
	*berth_id = func_p2be(r.pos);
	r.good_taken = -1;
	r.last_dir = -1;
	
}

// compute the score of each direction to choose a better one
double move(Robot &r, int direction){
	if (!r.running) return ninf; // robot not running
	int x = r.pos.x + mv[direction][0];
	int y = r.pos.y + mv[direction][1];
	if (func_outside_map) return ninf; // outside the map
	if (env[x][y] == '#' || env[x][y] == '*') return ninf; // not a land or a berth
	if (p2r[x][y] >= 0) return ninf;// occupied by other robots
	double score = 0;
	if (r.good_taken != -1){
		score = func_eval_to_berth(r.pos, Position(x, y), r.target_berths);
	} else {
		score = func_eval_to_good(r.pos, Position(x, y), r.target_goods);
	}
	if (r.last_dir == direction) score *= 1.001; // encourage to keep diretcion
	else if (r.last_dir ^ direction == 1) score *= 0.001; // discourage to turn back
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
	// input map
    for(int i = 0; i < len_env; i++){
    	for(int j = 0; j < len_env; j++){
    		scanf("%c", &env[i][j]);
		}
	}
	// input information of the berth
    for(int i = 0; i < n_be; i++)
    {
        int id;
        scanf("%d", &id);
        berths[id].id = id;
        scanf("%d%d%d%d", &berths[id].pos.x, &berths[id].pos.y, &berths[id].ttime, &berths[id].lspeed);
    	get_berth_distance_matrix(berths[id].pos, berths[id].dis);
	}
	for(int i = 0; i < n_r; i++){
		robots[i].id = i;
	}
	int boat_capacity;
    scanf("%d", &boat_capacity);
	for(int i = 0; i < n_bo; i++){
		boats[i].cap = boat_capacity;
	}
    char okk[100];
    scanf("%s", okk);
    puts("OK");
    fflush(stdout);
}

int Input()
{
	int frame_id, money;
    scanf("%d%d", &frame_id, &money);
    // judging if goods had disappeared
    for(vector<Good>::iterator g = goods.begin(); g != goods.end(); g++){
    	if (!available(*g, frame_id)) p2i[g->pos.x][g->pos.y] = -1;
	}
	// Goods Information
    int num;
    scanf("%d", &num);
    for(int i = 0; i < num; i ++)
    {
        int x, y, val;
        Good temp;
		scanf("%d%d%d", &x, &y, &val);
        temp.id = goods.size();
        temp.pos.x = x;
        temp.pos.y = y;
        temp.value = val;
        temp.itime = frame_id;
        goods.push_back(temp);
        p2i[x][y] = temp.id;
        get_goods_distance_matrix(temp.pos, temp.dis);
    }
    // Robots Information
    for(int i = 0; i < n_r; i++)
    {	int good_taken, status;
        scanf("%d%d%d%d", &good_taken, &robots[i].pos.x, &robots[i].pos.y, &status);
        if (!good_taken) robots[i].good_taken = -1;
        if(status == 1) robots[i].running = true;
        else robots[i].running = false;
        if (robots[i].running) {
        	if (robots[i].good_taken == -1){
        		robots[i].target_goods.clear();
        		for(vector<Good>::iterator g = goods.begin(); g != goods.end(); g++){
					robots[i].target_goods.push_back(g->id);
				}
			} else {
				robots[i].target_berths.clear();
				for(int j = 0; j < n_be; j++){
					if (p2b[berths[i].pos.x][berths[i].pos.y] != p2b[robots[i].pos.x][robots[i].pos.y]) continue;
					robots[i].target_berths.push_back(berths[i].id);
				}
			}
		}
    }
    // Boats Informatipn
    for(int i = 0; i < n_bo; i ++)
        scanf("%d%d\n", &boats[i].status, &boats[i].pos);
    char okk[100];
    scanf("%s", okk);
    return frame_id;
}

// some utils to solve the problem
void robot_dispatch(int frame_id){
	int i, good_id, berth_id;
	for(i = 0; i < n_r; i++){
		if (frame_id < i) continue;
		good_id = get(robots[i]);
		if (good_id != -1){
			goods[good_id].taken = true;
			printf("get %d\n", i);
			fflush(stdout);
			continue;
		}
		
		pull(robots[i], &good_id, &berth_id);
		if (good_id != -1){
			berths[berth_id].goods_temp.push(good_id);
			printf("pull %d\n", i);
			fflush(stdout);
			continue;			
		}
		double max_ev = move(robots[i], 0), esti_values;
		int d = 0;
		for(int j = 1; j < 4; j++){
			esti_values = move(robots[i], j);
			if(esti_values > max_ev){
				max_ev = esti_values;
				d = j;
			}
		}
		if (max_ev > 0){
			robots[i].last_dir = d;
			p2r[robots[i].pos.x][robots[i].pos.y] = -1;
			p2r[robots[i].pos.x + mv[d][0]][robots[i].pos.y + mv[d][i]] = i;
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
	return  x.sum> y.sum;
}
void boat_dispatch(int frame_id){
	int i, j;
	for(i = 0; i < n_bo; i++){
		if (boats[i].pos == -1){
			boats[i].load = 0;
			continue;
		}
		if (boats[i].status == 1) berths[boats[i].pos].occupied = i;
		if (boats[i].load == boats[i].cap || berths[boats[i].pos].goods_temp.size() == 0){
			berths[boats[i].pos].occupied = -1;
			berths[boats[i].pos].reserved = -1;
			boats[i].status = 0;
			boats[i].pos = -1;
			printf("go %d\n", i);
			fflush(stdout);
		}
	}
	to_order ordered_berths[n_be];
	for(i = 0; i < n_be; i++){
		int sum = 0;
		for(j = 0; j < berths[i].goods_temp.size(); j++){
			int g_id = berths[i].goods_temp.front();
			berths[i].goods_temp.pop();
			berths[i].goods_temp.push(g_id);
			sum += goods[g_id].value;
		}
		ordered_berths[i].id = berths[i].id;
		ordered_berths[i].sum = sum;
	}
	sort(ordered_berths, ordered_berths + n_be, cmp);
	for(int k = 0; k < n_be; k++){
		i = ordered_berths[k].id;
		if(berths[i].reserved != -1 || berths[i].occupied != -1) continue;
		for(j = 0; j < n_bo; j++){
			if (boats[j].status == 1 && boats[j].pos == -1){
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
	puts("OK");
	fflush(stdout);
}
int main(){
    Init();
    for(int zhen = 1; zhen <= 15000; zhen ++)
    {
        int id = Input();
		solve_frame(id);
        puts("OK");
        fflush(stdout);
    }
	return 0;
}

