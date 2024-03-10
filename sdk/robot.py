import sys
from typing import List, Tuple

from constant import *
from variable import *
from function_utils import *

from position import Position

class Robot:
    id: int
    pos: Position
    running: bool
    good_taken: int
    last_dir: int
    target_goods: List[int]
    target_berths: List[int]
    def __init__(self, id: int, x: int = -1, y: int = -1):
        self.id = id
        self.pos = Position(x, y)
        self.running = True
        self.good_taken = -1
        self.last_dir = -1 # -1: not cared; 0/1/2/3: L/R/U/D
        self.target_goods = []
        self.target_berths = []
    
    def get(self) -> int:
        if not self.running: # robot not running
            return -1
        if self.good_taken != -1: # robot already with good taken
            return -1
        if p2i[self.pos.x][self.pos.y] == -1: # no good to take
            return -1
        self.good_taken = p2i[self.pos.x][self.pos.y]
        self.last_dir = -1
        return self.good_taken
    
    def pull(self) -> Tuple[int, int]:
        if not self.running: # robot not running
            return -1, -1
        if self.good_taken == -1: # robot with no good taken
            return -1, -1
        if env[self.pos.x][self.pos.y] != "B": # not on a berth
            return -1, -1
        good_id = self.good_taken # which good to pull
        berth_id = func_p2be(self.pos) # which berth to pull on
        self.good_taken = -1
        self.last_dir = -1
        return good_id, berth_id

    def move(self, direction: int) -> float:
        if not self.running: # robot not running
            return ninf
        x, y = self.pos.x + mv[direction][0], self.pos.y + mv[direction][1]
        if func_outside_map(x, y): # outside the map
            return ninf
        if env[x][y] == "#" or env[x][y] == "*": # not a land or a berth to move on
            return ninf
        if p2r[x][y] >= 0: # occupied by other robots
            return ninf
        score = 0
        if self.good_taken != -1: # to get a good
            score = func_eval_to_berth(self.pos, Position(x, y), self.target_berths)
        else: # to pull a good on a berth
            score = func_eval_to_good(self.pos, Position(x, y), self.target_goods)
        if self.last_dir == direction: # encourage to keep direction
            score *= 1.001
        elif (self.last_dir ^ direction) == 1: # discourage to turn back
            score *= 0.001
        return score