from constant import *

terminal_flag = False

env = [] # map information, 2-dimensions
berths = [] # berth instance list
robots = [] # robot instance list
boats = [] # boat instance list
goods = [] # good instance list

n_bl = 0 # number of blocks
p2b = [[0 for _ in range(len_env)] for _ in range(len_env)] # block index of each position
p2i = [[-1 for _ in range(len_env)] for _ in range(len_env)] # good index of each position
p2r = [[-1 for _ in range(len_env)] for _ in range(len_env)] # robot id of each position, -1 when not occupied

__all__ = [
    "terminal_flag", "env", "berths", "robots", "boats", "goods", "p2i", "n_bl", "p2b", "p2r"
]