import argparse
import os

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--day', type=str, default="3.7",
        help='Date of the testing map, e.g. `3.7`'
    ) # M.D
    args = parser.parse_args()
    day = args.day
    map_path = f"maps\map-{day}.txt"
    os.system(
        f"""
        PreliminaryJudge.exe -m {map_path} "cpp/main.exe"
        """
    )