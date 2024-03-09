import threading

from constant import *

from input_utils import *
from solve_utils import *

if __name__ == "__main__":
    input_map()

    for frame in range(n_f):
        try:
            t_input = threading.Thread(target=input_frame)
            t_input.start()
            t_input.join()
        except Exception:
            terminal_flag = True
            break

        try:
            t_solve = threading.Thread(target=solve_frame, args=(frame, ))
            t_solve.start()
            t_solve.join()
        except Exception:
            terminal_flag = True
            break