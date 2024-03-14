import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt("log.txt", delimiter=" ")

x = data[:, 0]
y = data[:, 1]

plt.plot(x, y)

plt.xlabel("x")
plt.xticks(list(range(0, 15000 + 1, 1000)), rotation=45)

plt.ylabel("y")

plt.show()
