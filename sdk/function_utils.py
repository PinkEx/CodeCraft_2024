import heapq
import sys
from typing import List

# import numpy as np

from constant import *
from variable import *

from position import Position

# def func_softmax(x):
#     x = np.array(x)
#     exp_x = np.exp(x - np.max(x))
#     return exp_x / exp_x.sum(axis=0, keepdims=True)

def func_outside_map(x: int, y: int):
    return x < 0 or x >= len_env or y < 0 or y >= len_env

def func_p2b(obj: object):
    if hasattr(obj, "pos"):
        return p2b[obj.pos.x][obj.pos.y]
    else:
        raise AttributeError
        
def func_p2be(pos: Position):
    for b in berths:
        if pos.x - b.pos.x >= 0 and pos.x - b.pos.x < len_berth \
        and pos.y - b.pos.y >= 0 and pos.y - b.pos.y < len_berth:
            return b.id

def func_eval_to_good(ori: Position, pos: Position, target_goods: List[int]):
    scores = [
        1.0 / (goods[g].dis[pos.x][pos.y] + 0.5) - 1.0 / (goods[g].dis[ori.x][ori.y] + 0.5)
        for g in target_goods
    ]
    scores = heapq.nlargest(8, scores)
    scores.reverse()
    score = 0
    for s in scores:
        score = score * gamma + max(s, 0)
    return score

def func_eval_to_berth(ori: Position, pos: Position, target_berths: List[int]):
    scores = [
        1.0 / (berths[b].dis[pos.x][pos.y] + 0.5) - 1.0 / (berths[b].dis[ori.x][ori.y] + 0.5)
        for b in target_berths
    ]
    scores = heapq.nlargest(8, scores)
    scores.reverse()
    score = 0
    for s in scores:
        score = score * gamma + max(s, 0)
    return score