from collections import deque
from typing import List
from itertools import repeat

import numpy as np

from constant import *
from variable import *
from function_utils import *

from position import Position

class Good:
    id: int
    pos: Position
    value: int
    itime: int
    dis: List[List[int]]
    taken: bool
    def __init__(self, id: int, x: int, y: int, value: int, itime: int):
        self.id = id
        self.pos = Position(x, y)
        self.value = value
        self.itime = itime
        self.taken = False
        self.dis = [[inf for _ in range(len_exp)] for _ in range(len_exp)]

    def available(self, frame: int):
        return frame - self.itime < len_window and not self.taken
    
    def explorable(self, pos: Position):
        return abs(self.pos.x - pos.x) <= 50 and abs(self.pos.y - pos.y) <= 50

    def get_distance_matrix(self):
        queue = deque([self.pos])
        self.dis[0][0] = 0
        while queue:
            p = queue.popleft()
            if self.dis[p.x - self.pos.x][p.y - self.pos.y] > 100: return
            for direction in range(4):
                x, y = p.x + mv[direction][0], p.y + mv[direction][1]
                if func_outside_map(x, y): # outside the map
                    continue
                if not self.explorable(Position(x, y)): # not explorable
                    continue
                if env[x][y] == "#" or env[x][y] == "*": # not a land or a berth to move on
                    continue 
                if self.dis[x - self.pos.x][y - self.pos.y] >= inf:
                    self.dis[x - self.pos.x][y - self.pos.y] = self.dis[p.x - self.pos.x][p.y - self.pos.y] + 1
                    queue.append(Position(x, y))
