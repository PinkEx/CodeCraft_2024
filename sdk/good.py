from collections import deque
from typing import List

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
        self.dis = [[inf for _ in range(len_env)] for _ in range(len_env)]

    def available(self, frame: int):
        return frame - self.itime < len_window and not self.taken

    def get_distance_matrix(self, frame_id: int):
        queue = deque([self.pos])
        self.dis[self.pos.x][self.pos.y] = 0
        while queue:
            p = queue.popleft()
            if self.dis[p.x][p.y] >= 80: return
            for direction in range(4):
                x, y = p.x + mv[direction][0], p.y + mv[direction][1]
                if func_outside_map(x, y): # outside the map
                    continue
                if env[x][y] == "#" or env[x][y] == "*": # not a land or a berth to move on
                    continue
                if self.dis[x][y] >= inf:
                    self.dis[x][y] = self.dis[p.x][p.y] + 1
                    queue.append(Position(x, y))
