import json
import matplotlib.pyplot as plt

log_id = input()

# 读取数据
data = []
with open(f"logs/log_{log_id}.txt", "r") as file:
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

# Plotting
fig, ax1 = plt.subplots(figsize=(10, 5))

color = 'tab:blue'
ax1.set_xlabel('ID')
ax1.set_ylabel('Boat Capacity', color=color)
ax1.plot(ids, boat_capacities, marker='o', linestyle='-', color=color, label='Boat Capacity')
ax1.tick_params(axis='y', labelcolor=color)
ax1.set_ylim(40, 200)

ax2 = ax1.twinx()  
color = 'tab:green'
ax2.set_ylabel('Sum Value and Score', color=color)
ax2.plot(ids, sum_values, marker='o', linestyle='-', color=color, label='Sum Value')
ax2.plot(ids, scores, marker='o', linestyle='-', color='orange', label='Score')
ax2.plot(ids, diff_values, marker='o', linestyle='-', color='red', label='Difference')
ax2.tick_params(axis='y', labelcolor=color)

fig.tight_layout()  
plt.title('Data Visualization')
fig.legend(loc='upper right')
plt.grid(True)
plt.show()