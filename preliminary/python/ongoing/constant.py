n_be = 10 # number of berths
n_r = 10 # number of robots
n_bo = 5 # number of boats
n_f = 15000 # number of frames
len_env = 200 # len of the map
len_exp = 101 # len of the exploration window
len_window = 1000 # len of the goods' appearing window
len_berth = 4 # len of a berth

gamma = 0.3 # candidate decaying rate
inf = int(1e7) # infinity
ninf = -int(1e7) # negative infinity

mv = [[0, 1], [0, -1], [-1, 0], [1, 0]] # moving shifts: R/L/U/D