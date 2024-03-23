import json
import matplotlib.pyplot as plt
import numpy as np

# 读取文件并提取数据
def read_file(file_path):
    scores = []
    with open(file_path, "r") as file:
        for line in file:
            line = line.strip()
            if line.isdigit():  # 忽略空行
                pass
            elif line != "":
                json_data = json.loads(line)
                score = json_data.get('score')  # 提取 score 值
                if score is not None:
                    scores.append(score)
    return scores

# 计算平均值
def calculate_average(scores):
    return np.mean(scores)


n_seed = 50

avg_by_seed_by_version = []

for ver_id in range(11, 16):
    score_by_seed = [[] for i in range(n_seed)]
    for map_id in range(1, 9):
        sc = read_file(f"logs_cpp_{ver_id}/log_{map_id}.txt")
        for seed in range(n_seed):
            score_by_seed[seed].append(sc[seed])
    avg_by_seed = [sum(score_by_seed[seed]) / len(score_by_seed[seed]) for seed in range(n_seed)]
    avg_by_seed_by_version.append(avg_by_seed[:])

mx_avg_by_seed_by_version = []
for seed_id in range(n_seed):
    mx = 0
    for a in avg_by_seed_by_version:
        mx = max(mx, a[seed_id])
    mx_avg_by_seed_by_version.append(mx)

print(mx_avg_by_seed_by_version)

# # 设置ID
ids = list(range(1, n_seed + 1))

# 绘制折线图
# plt.plot(ids, avg_by_seed_by_version[0], marker='o', linestyle='-', color="tab:blue", label='ver. 11')
plt.plot(ids, avg_by_seed_by_version[1], marker='d', linestyle='-', color="tab:orange", label='ver. 12')
plt.plot(ids, avg_by_seed_by_version[2], marker='s', linestyle='-', color="tab:green", label='ver. 13')
plt.plot(ids, avg_by_seed_by_version[3], marker='*', linestyle='-', color="tab:red", label='ver. 14')
plt.plot(ids, avg_by_seed_by_version[4], marker='x', linestyle='-', color="tab:purple", label='ver. 15')
# plt.plot(ids, mx_avg_by_seed_by_version, marker='o', linestyle='-', color="black", label='ver. max per seed')

# 添加标题和标签
plt.title('Avg Score Per SeedID - Multi Ver.')
plt.xlabel('ID')
plt.ylabel('Score')

# 添加图例
plt.legend()

# 显示图形
plt.show()
