import matplotlib.pyplot as plt
import numpy as np

arr = np.loadtxt("temp_stick_2024-03-22.txt", delimiter=",", dtype=str)

temperature = arr[:, 1].astype('float64')
umidity = arr[:, 2].astype('float64')
pressure = arr[:, 3].astype('float64')
probe = arr[:, 4].astype('float64')

fig, ax = plt.subplots(4,1)
ax[0].plot(temperature)
ax[0].set_title("Temperature")
ax[1].plot(umidity)
ax[1].set_title("Umidity")
ax[2].plot(pressure)
ax[2].set_title("Pressure")
ax[3].plot(probe)
ax[3].set_title("Probe Temperature")

fig.tight_layout(pad=1.5)
fig.savefig("test.png")

plt.show()