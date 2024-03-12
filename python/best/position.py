from constant import *

class Position:
    x: int
    y: int
    genre: int # -1: undefine/not used, 0: land, 1: sea, 2: barrier
    def __init__(self, x: int, y: int, genre: int = -1):
        self.x = x
        self.y = y
        self.genre = genre