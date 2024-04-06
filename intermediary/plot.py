import matplotlib.pyplot as plt

# 从文件中读取数据
with open('logs/log1.txt', 'r') as file:
    data = [line.strip() for line in file]

# 初始化频率统计
plus_counts = [0] * 201
multiply_counts = [0] * 201

# 统计加号开头和乘号开头数字的频率
for num in data:
    if num == '' or num[0] not in ['+', '*']: continue
    if num[0] == '+':
        plus_counts[int(num[1:])] += 1
    elif num[0] == '*':
        multiply_counts[int(num[1:])] += 1

# 绘制分布图
plt.figure(figsize=(10, 5))

plt.subplot(1, 2, 1)
plt.bar(range(201), plus_counts, color='green')
plt.xlabel('Value')
plt.ylabel('Frequency')
plt.xticks(range(0, 201, 20))
plt.yticks(range(0, 51, 10))
plt.title('Distribution of Values Appeared(+)')

plt.subplot(1, 2, 2)
plt.bar(range(201), multiply_counts, color='orange')
plt.xlabel('Value')
plt.ylabel('Frequency')
plt.xticks(range(0, 201, 20))
plt.yticks(range(0, 51, 10))
plt.title('Distribution of Value Took Effect(*)')

plt.tight_layout()
plt.show()