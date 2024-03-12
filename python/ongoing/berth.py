from collections import deque
from typing import List

from constant import *
from variable import *
from function_utils import *

from position import Position

class Berth:
    id: int
    pos: Position
    ttime: int
    lspeed: int
    reserved: int # -1: not reserverd; otherwise: id of the boat
    occupied: int # -1: not occupied; otherwise: id of the boat
    goods_temp: List[int]
    dis: List[List[int]]
    def __init__(self, id: int, x: int, y: int, ttime: int, lspeed: int):
        self.id = id
        self.pos = Position(x, y)
        self.ttime = ttime
        self.lspeed = lspeed
        self.reserved = -1
        self.occupied = -1
        self.goods_temp = []
        self.dis = [[inf for _ in range(len_env)] for _ in range(len_env)]
    
    def get_distance_matrix(self):
        queue = deque([])
        for dx in range(len_berth):
            for dy in range(len_berth):
                self.dis[self.pos.x + dx][self.pos.y + dy] = 0
                queue.append(Position(self.pos.x + dx, self.pos.y + dy))
        while queue:
            p = queue.popleft()
            for direction in range(4):
                x, y = p.x + mv[direction][0], p.y + mv[direction][1]
                if func_outside_map(x, y): # outside the map
                    continue
                if env[x][y] == "#" or env[x][y] == "*": # not a land or a berth to move on
                    continue
                if self.dis[x][y] >= inf:
                    self.dis[x][y] = self.dis[p.x][p.y] + 1
                    queue.append(Position(x, y))

    def load(self):
        for i in range(self.lspeed):
            if len(self.goods_temp) > 0 and boats[self.occupied].load < boats[self.occupied].cap:
                g = self.goods_temp.pop(0)
                boats[self.occupied].load += 1
            else: return