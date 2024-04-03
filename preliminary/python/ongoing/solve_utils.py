import random
import sys

# import numpy as np

from constant import *
from variable import *

from function_utils import *

def robot_dispatch(frame_id: int):
    for i in range(n_r):
        p2r[robots[i].pos.x][robots[i].pos.y] = i

    for i in range(n_r):
        if frame_id < i: # async starts
            continue
        good_id = robots[i].get()
        if good_id != -1:
            goods[good_id].taken = True
            print("get", i)
            sys.stdout.flush()
            continue
        
        good_id, berth_id = robots[i].pull()
        if good_id != -1:
            berths[berth_id].goods_temp.append(good_id)
            print("pull", i)
            sys.stdout.flush()
            continue

        esti_values = [
            robots[i].move(d) for d in range(4)
        ]
        max_ev = max(esti_values)
        if max_ev > 0:
            d = esti_values.index(max_ev)
            robots[i].last_dir = d
            p2r[robots[i].pos.x][robots[i].pos.y] = -1
            p2r[robots[i].pos.x + mv[d][0]][robots[i].pos.y + mv[d][1]] = i
            print("move", i, d)
            sys.stdout.flush()

def boat_dispatch(frame_id: int):
    ordered_berths = sorted(
        berths,
        key=lambda b: sum([goods[g].value for g in b.goods_temp]), # len(b.goods_temp),
        reverse=True
    )

    for i in range(n_bo):
        if boats[i].pos == -1:
            boats[i].load = 0
            continue
        if boats[i].status == 1:
            berths[boats[i].pos].occupied = i
        if berths[boats[i].pos].occupied == i and (boats[i].load == boats[i].cap or len(berths[boats[i].pos].goods_temp) == 0):
            # time_arrival = frame_id + berths[boats[i].pos].ttime
            # sys.stderr.write(f"boat {i} has a cap of {boats[i].cap}, with a load of {boats[i].load} @ from frame {frame_id} to frame {time_arrival}\n")
            berths[boats[i].pos].occupied = -1
            berths[boats[i].pos].reserved = -1
            if boats[i].load < boats[i].cap * 4 / 5 and frame_id <= 10000:
                flag = False
                for k in range(n_be):
                    j = ordered_berths[k].id
                    if berths[j].reserved != -1 or berths[j].occupied != -1: continue
                    flag = True
                    berths[j].reserved = i
                    boats[i].pos = j
                    print("ship", i, j)
                    sys.stdout.flush()
                    break
                if flag: continue
            boats[i].status = 0
            boats[i].pos = -1
            print("go", i)
            sys.stdout.flush()

    for k in range(n_be):
        i = ordered_berths[k].id
        if berths[i].reserved != -1 or berths[i].occupied != -1: continue
        for j in range(n_bo):
            if boats[j].status == 1 and boats[j].pos == -1:
                berths[i].reserved = j
                boats[j].pos = i
                print("ship", j, i)
                sys.stdout.flush()
                break

    for i in range(n_be):
        if berths[i].occupied == -1: continue
        berths[i].load()

def solve_frame(frame_id: int):
    robot_dispatch(frame_id)
    boat_dispatch(frame_id)
    
    sys.stderr.write(f"[FRAME {frame_id}]\n")
    for b in berths:
        sum_values = sum([goods[g].value for g in b.goods_temp])
        sys.stderr.write(f"berth {b.id} occupied by {b.occupied} of total value {sum_values}, reserved by {b.reserved}: {b.goods_temp}\n")
        if b.reserved != -1:
            sys.stderr.write(f"reserver: boat {b.reserved} (state: {boats[b.reserved].status} to berth {boats[b.reserved].pos})\n")
        if b.occupied != -1:
            sys.stderr.write(f"reserver: boat {b.occupied} (state: {boats[b.occupied].status} to berth {boats[b.occupied].pos})\n")

    print("OK")
    sys.stdout.flush()