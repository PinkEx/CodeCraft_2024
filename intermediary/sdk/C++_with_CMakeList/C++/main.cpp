#include <bits/stdc++.h>
using namespace std;
const int N = 200;

int robot_num = 0;
int boat_num = 0;
int berth_num = 0;

int goods_num = 0;
int frame_id = 0;
int money, boat_capacity, boat_price=8000, robot_price=2000;

char grid[N][N];


struct Robot
{
    int id, x, y;
    int goods_num;
}robot[20];


struct Berth
{
    int x, y;
    int loading_speed;
    Berth(){}
    Berth(int x, int y, int loading_speed) {
        this -> x = x;
        this -> y = y;
        this -> loading_speed = loading_speed;
    }
}berth[10];


struct Boat
{
    int id, x, y, dir;
    int goods_num, status;
    Boat() {}
    Boat(int startX, int startY) {
        x = startX;
        y = startY;
    }
}boat[10];

vector<pair<int, int>> robot_purchase_point;
vector<pair<int, int>> boat_purchase_point;
vector<pair<int, int>> delivery_point;
void ProcessMap()
{
    for(int i = 0; i < N; i ++){
        for(int j = 0; j < N; j ++){
            if(grid[i][j] == 'R')
                robot_purchase_point.push_back(make_pair(i, j));
            else if(grid[i][j] == 'S')
                boat_purchase_point.push_back(make_pair(i, j));
            else if(grid[i][j] == 'T')
                delivery_point.push_back(make_pair(i, j));
        }
    }
}


void Init()
{
    for(int i = 0; i < N; i ++){
        scanf("%s", grid[i]);
    }
    ProcessMap();
    scanf("%d", &berth_num);
    for(int i = 0; i < berth_num; i ++)
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d", &berth[id].x, &berth[id].y, &berth[id].loading_speed);
    }
    scanf("%d", &boat_capacity);
    char okk[100];
    scanf("%s", okk);
    printf("OK\n");
    fflush(stdout);
}


int Input()
{
    scanf("%d", &money);

    scanf("%d", &goods_num);
    for(int i = 0; i < goods_num; i ++)
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);
        if(val == 0) val = 0;
    }

    scanf("%d", &robot_num);
    for(int i = 0; i < robot_num; i ++)
        scanf("%d%d%d%d", &robot[i].id, &robot[i].goods_num, &robot[i].x, &robot[i].y);

    scanf("%d", &boat_num);
    for(int i = 0; i < boat_num; i ++)
        scanf("%d%d%d%d%d%d\n", &boat[i].id, &boat[i].goods_num, &boat[i].x, &boat[i].y, &boat[i].dir, &boat[i].status);
    char okk[100];
    scanf("%s", okk);
}


// -----------------------------------------------start----------------------------------------
int main()
{
    Init();
    while(scanf("%d", &frame_id) != EOF)
    {
        Input();
        if(money >= robot_price && robot_num <= 1){
            printf("lbot %d %d\n", robot_purchase_point[0].first, robot_purchase_point[0].second);
        }

        if(money >= boat_price && boat_num <= 1){
            printf("lboat %d %d\n", boat_purchase_point[0].first, boat_purchase_point[0].second);
        }

        for(int i = 0; i < robot_num; i ++)
            printf("move %d %d\n", i, rand() % 4);

        for(int i = 0; i < boat_num; i ++){
            int status = abs(rand()) % 2;
            if(status == 1)
                printf("ship %d\n", i);
            else
                printf("rot %d %d\n", i, rand() % 2);
        }
        puts("OK");
        fflush(stdout);
    }
    return 0;
}