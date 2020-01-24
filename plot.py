import matplotlib.pyplot as plt

# Serial time 
s = 1061.643632

o = [1106.626574, 616.610053, 516.672035, 450.111087, 391.400046]

p = [836.753784, 436.502735, 256.854246, 316.143415, 300.060989]

# Calculating Speedup
oprime = [s/x for x in o]
pprime = [s/x for x in p]

# print(oprime)
print(pprime)

x = [1, 2, 4, 8, 16]

# Calculating Efficiency
for i in range(0, 5):
    oprime[i] = oprime[i]/x[i]
    pprime[i] = pprime[i]/x[i]

# print(oprime)
print(pprime)
# plotting the points  
plt.plot(x, pprime, linestyle='--', marker='o', color='r') 
  
# naming the x axis 
plt.xlabel('Number of threads') 
# naming the y axis 
plt.ylabel('Efficiency of Pthread') 

# giving a title to my graph 
plt.title('parallel efficieny vs number of threads') 

# plt.show()
plt.savefig('Pthread.pdf')
