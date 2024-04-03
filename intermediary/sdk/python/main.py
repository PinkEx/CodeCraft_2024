
import sys
import random


N = 200

robot_num = 20
boat_num = 10
berth_num = 10

goods_num = 0
frame_id = 0
money = 0
boat_capacity = 0
boat_price=8000
robot_price=2000

grid = []

class Robot:
    def __init__(self, id=0, startX=0, startY=0, goods=0):
        self.id = id
        self.x = startX
        self.y = startY
        self.goods = goods

robot = [Robot() for _ in range(robot_num)]

class Berth:
    def __init__(self, x=0, y=0, loading_speed=0):
        self.x = x
        self.y = y
        self.loading_speed = loading_speed

berth = [Berth() for _ in range(berth_num)]

class Boat:
    def __init__(self, id=0, x=0, y=0, dir=0, num=0, status=0):
        self.id = id
        self.num = num
        self.x = x
        self.y = y
        self.dir = dir
        self.status = status

boat = [Boat() for _ in range(10)]

robot_purchase_point = []
boat_purchase_point = []
delivery_point = []

def process_map(grid):
    for i in range(N):
        for j in range(N):
            if grid[i][j] == 'R':
                robot_purchase_point.append((i, j))
            elif grid[i][j] == 'S':
                boat_purchase_point.append((i, j))
            elif grid[i][j] == 'T':
                delivery_point.append((i, j))

def Init():
    for i in range(0, N):
        line = input()
        grid.append(list(line))
    process_map(grid)
    berth_num = int(input())
    for i in range(berth_num):
        id, x, y, loading_speed = map(int, input().split())
        berth.append(Berth(x, y, loading_speed))

    boat_capacity = int(input())
    okk = input()
    print("OK")


def Input():
    goods_num = int(input())
    for i in range(goods_num):
        x, y, val = map(int, input().split())
        grid[x][y] = val

    global robot_num
    robot_num = int(input())
    for i in range(robot_num):
        robot[i].id, robot[i].goods, robot[i].x, robot[i].y = map(int, input().split())

    global boat_num
    boat_num = int(input())
    for i in range(boat_num):
        boat[i].id, boat[i].goods_num, boat[i].x, boat[i].y, boat[i].dir, boat[i].status = map(int, input().split())
    okk = input()


if __name__ == "__main__":
    Init()
    while True:
        try:

            frame_id, money = map(int, input().split(" "))
            Input()
            if robot_num <= 1:
                sys.stderr.write(f"lbot ({robot_purchase_point[0][0]}, {robot_purchase_point[0][1]})!\n")
                print("lbot", robot_purchase_point[0][0], robot_purchase_point[0][1])

            if boat_num <= 1:
                print("lboat", boat_purchase_point[0][0], boat_purchase_point[0][1])

            for i in range(robot_num):
                print("move", i, random.randint(0, 3))

            for i in range(boat_num):
                status = random.randint(0, 1)
                if status == 0:
                    print("ship", i)
                else:
                    print("rot", i, random.randint(0, 1))

            print("OK")
            sys.stdout.flush()

        except EOFError:
            break

