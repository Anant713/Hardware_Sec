import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv('/home/anant/Hardware_sec/result.txt', header=None)
df.columns = ["Buffer size", "Time", "Latency"]


avg = df.groupby("Buffer size")["Latency"].mean().reset_index()


plt.plot(avg["Latency"],avg["Buffer size"], marker='s', color='red', label='Averaged')


plt.ylabel('Buffer Size (KB)')
plt.xlabel('Latency (ns)')
plt.title('Memory Latency vs Buffer Size')
plt.grid(True)
plt.savefig("Mmeorv vs latency")
plt.show()
