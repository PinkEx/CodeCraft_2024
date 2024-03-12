from collections import deque
import sys

from constant import *
from variable import *
from function_utils import *

from boat import Boat
from berth import Berth
from good import Good
from position import Position
from robot import Robot

def search_block(x: int, y: int, bindex: int):
    p2b[x][y] = bindex
    queue = deque([Position(x, y)])
    while queue:
        p = queue.popleft()
        for direction in range(4):
            x, y = p.x + mv[direction][0], p.y + mv[direction][1]
            if func_outside_map(x, y): # outside the map
                continue
            if env[x][y] == "#" or env[x][y] == "*": # not a land or a berth to move on
                continue
            if p2b[x][y] == 0:
                p2b[x][y] = bindex
                queue.append(Position(x, y))

def preprocess():
    global n_bl
    for i in range(len_env):
        for j in range(len_env):
            if env[i][j] == "#" or env[i][j] == "*":
                p2b[i][j] = -1
                continue
            if p2b[i][j] == 0:
                n_bl += 1
                search_block(i, j, n_bl)

def input_map():
    for i in range(len_env):
        line = input()
        env.append(list(line))

    # Berths Information
    for i in range(n_be):
        line = input()
        parsed_line = [int(c) for c in line.split(sep=" ")]
        berths.append(
            Berth(
                id=parsed_line[0],
                x=parsed_line[1],
                y=parsed_line[2],
                ttime=parsed_line[3],
                lspeed=parsed_line[4],
            )
        )
        berths[-1].get_distance_matrix()

    # Robots Information
    for i in range(n_r):
        robots.append(
            Robot(
                id=i
            )
        )
    
    # Boats Information
    boat_cap = int(input())
    for i in range(n_bo):
        boats.append(
            Boat(
                id=i,
                cap=boat_cap,
                pos=-1,
                status=1
            )
        )

    preprocess()

    OK = input()
    print("OK")
    sys.stdout.flush()

def input_frame() -> int:
    try:
        frame_id, money = map(int, input().split(" "))
    except EOFError:
        terminal_flag = True
        return -1

    for g in goods:
        if not g.available(frame=frame_id) and p2i[g.pos.x][g.pos.y] == g.id:
            p2i[g.pos.x][g.pos.y] = -1
    
    # Goods Information
    n_g_new = int(input())
    for i in range(n_g_new):
        x, y, value = map(int, input().split(" "))
        if value < 50: # good of low value
            continue
        goods.append(
            Good(
                id=len(goods),
                x=x,
                y=y,
                value=value,
                itime=frame_id
            )
        )
        p2i[x][y] = goods[-1].id
        goods[-1].get_distance_matrix(frame_id)

    # Robots Information
    for i in range(n_r):
        good_taken, x, y, status = map(int, input().split(" "))
        if good_taken == 0:
            robots[i].good_taken = -1
        robots[i].pos = Position(x, y)
        robots[i].running = (status == 1)
        if robots[i].running == 1: # running normally
            if robots[i].good_taken == -1:
                new_target_goods = []
                for g in goods:
                    if g.available(frame=frame_id) and (g.dis[x][y] <= 400 or g.dis[x][y] < inf and len(new_target_goods) == 0):
                        new_target_goods.append(g.id)
                robots[i].target_goods = new_target_goods[:]
            else:
                robots[i].target_berths = []
                for b in berths:
                    if func_p2b(b) != func_p2b(robots[i]): # not accessible
                        continue
                    robots[i].target_berths.append(b.id)

    # Boats Information
    for i in range(n_bo):
        status, pos = map(int, input().split(" "))
        boats[i].status = status
        boats[i].pos = pos

    OK = input()
    return frame_id
