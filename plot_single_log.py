import json
import matplotlib.pyplot as plt
import numpy as np

log_id = input()

# 读取数据
data = []
with open(f"logs_cpp_12/log_{log_id}.txt", "r") as file:
    lines = file.readlines()
    current_data = {}
    for line in lines:
        line = line.strip()
        if line.isdigit():  # 检查是否为数字，作为新数据的ID
            if current_data:  # 确保上一个数据已经收集完整
                data.append(current_data)
            current_data = {"id": int(line)}
        elif line != "":
            print(line)
            # 解析JSON数据并添加到当前数据中
            current_data.update(json.loads(line))

# 添加最后一个数据
if current_data:
    data.append(current_data)

# 提取数据
ids = [entry["id"] for entry in data]
boat_capacities = [entry["boat_capacity"] for entry in data]
sum_values = [entry["sum_value"] for entry in data]
scores = [entry["score"] for entry in data]
diff_values = [sum_value - score for sum_value, score in zip(sum_values, scores)]

# 计算平均值
avg_boat_capacity = np.mean(boat_capacities)
avg_sum_value = np.mean(sum_values)
avg_score = np.mean(scores)
avg_diff_value = np.mean(diff_values)

# avg_score1 = np.mean(scores[:1] + scores[:23])
# print(avg_score1)

# Plotting
fig, ax1 = plt.subplots(figsize=(9, 6))

ax1.set_xlabel('ID')
ax1.set_ylabel('Boat Capacity', color="black")
ax1.plot(ids, boat_capacities, marker='o', linestyle='-', color="tab:blue", label='Boat Capacity')
ax1.axhline(y=avg_boat_capacity, color='tab:blue', linestyle='--', label=f'Average: {avg_boat_capacity:.2f}')
ax1.tick_params(axis='y', labelcolor="black")
ax1.set_ylim(40, 200)

ax2 = ax1.twinx()
ax2.set_ylabel('Sum Value and Score', color="black")
ax2.plot(ids, sum_values, marker='o', linestyle='-', color="tab:green", label='Sum Value')
ax2.axhline(y=avg_sum_value, color='green', linestyle='--', label=f'Average: {avg_sum_value:.2f}')
ax2.plot(ids, scores, marker='o', linestyle='-', color='orange', label='Score')
ax2.axhline(y=avg_score, color='tab:orange', linestyle='--', label=f'Average: {avg_score:.2f}')
ax2.plot(ids, diff_values, marker='o', linestyle='-', color='red', label='Difference')
ax2.axhline(y=avg_diff_value, color='tab:red', linestyle='--', label=f'Average: {avg_diff_value:.2f}')
ax2.tick_params(axis='y', labelcolor="black")

fig.tight_layout()  
plt.title('Data Visualization')
fig.legend(loc='upper right', bbox_to_anchor=(0.75, 0.75))
plt.grid(True)
plt.show()