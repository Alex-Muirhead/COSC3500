import numpy as np
import h5py
import matplotlib.pyplot as plt
import matplotlib.animation as animation


FILE = "particles.h5"
DATASET = "DS1"


hf = h5py.File(FILE, 'r')
time_series = hf[DATASET]
time_series = np.array(time_series)

N, T = time_series.shape

grid_size = int(np.sqrt(N)*2)
# random non-repetitive coordinates
coords = np.array([(i, j) for i in range(grid_size) for j in range(grid_size)])
idx = np.random.choice(grid_size*grid_size, size=N, replace=False)
coords = coords[idx,:]

# plot the first frame and save the markers
fig = plt.figure()
marker = [0]*N
for i, coord in enumerate(coords):
    marker[i] = plt.scatter(coord[0], coord[1], color='blue', alpha=1, s=10**2)

def update_plot(t): 
    for i in range(N):
        if time_series[i][t] > 0.5:
            alpha = 1
        else: # past half-life
            alpha = 0.5
            marker[i].set_alpha(alpha)

    plt.title(f't={t}')

anim = animation.FuncAnimation(fig, update_plot, frames=range(T))
anim.save("decay.gif")
