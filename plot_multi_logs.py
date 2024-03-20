import json
import matplotlib.pyplot as plt
import numpy as np

id = input()
# 读取文件并提取数据
def read_file(file_path):
    scores = []
    with open(file_path + f"log_{id}.txt", 'r') as file:
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

# 读取三个文件的数据
scores1 = read_file('logs_cpp_11/')
scores2 = read_file('logs_cpp_12/')
scores3 = read_file('logs_cpp_13/')
scores4 = read_file('logs_cpp_14/')
scores5 = read_file('logs/')

# 计算平均值
average1 = calculate_average(scores1)
average2 = calculate_average(scores2)
average3 = calculate_average(scores3)
average4 = calculate_average(scores4)
average5 = calculate_average(scores5)

# 设置ID
ids = list(range(1, len(scores1) + 1))

# 绘制折线图
plt.plot(ids, scores1, color="tab:blue", label='ver. 11')
plt.plot(ids, scores2, color="tab:orange", label='ver. 12')
plt.plot(ids, scores3, color="tab:green", label='ver. 13')
plt.plot(ids, scores4, color="tab:red", label='ver. 14')
plt.plot(ids, scores5, color="tab:purple", label='ver. 15')

plt.axhline(y=average1, color='blue', linestyle='--', label='Average ver. 11')
plt.axhline(y=average2, color='orange', linestyle='--', label='Average ver. 12')
plt.axhline(y=average3, color='green', linestyle='--', label='Average ver. 13')
plt.axhline(y=average4, color='red', linestyle='--', label='Average ver. 14')
plt.axhline(y=average5, color='purple', linestyle='--', label='Average ver. 15')

# 添加标题和标签
plt.title('Score Per SeedID - Multi Ver.')
plt.xlabel('ID')
plt.ylabel('Score')

# 添加图例
plt.legend()

# 显示图形
plt.show()
