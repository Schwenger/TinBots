import numpy as np
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
import math

data = []
with open("victimdirectionbad.csv") as file:
	for line in file:
		data.append(tuple(map(float, line.split(','))))
		
conv_data = list(map(lambda x: ((x[0] - x[1] + math.pi) % (2 * math.pi) - math.pi), data))
n ,bins, patches = plt.hist(conv_data, int( math.pi * 10), facecolor='green', alpha=0.5)
plt.savefig("output.pdf", format='pdf')
plt.show()

#print(list(conv_data))
