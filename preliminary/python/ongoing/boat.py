class Boat:
    id: int
    pos: int # INT-typed in first stage of competition
    cap: int # total capacity
    load: int # current load
    status: int # 0: moving, 1: loading or at virtual point, 2: wating outside a berth
    def __init__(self, id: int, cap: int, pos: int, status: int):
        self.id = id
        self.cap = cap
        self.pos = pos
        self.load = 0
        self.status = status
