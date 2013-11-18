import matplotlib.pyplot as plt

fig = plt.figure(1)

plt.title('Plot of reduce')
plt.plot([1000, 10000, 100000, 1000000, 10000000],
         [0.000081989, 0.000241044, 0.00125717, 0.0115089, 0.114808], 'b', 
         label= "1 thread")

plt.plot([1000, 10000, 100000, 1000000, 10000000],
         [0.000531936, 0.00131929, 0.00544347, 0.0457089, 0.28185], 'g', 
         label= "2 threads")

plt.plot([1000, 10000, 100000, 1000000, 10000000],
         [0.00042156, 0.00105977, 0.00458366, 0.0541587, 0.229545], 'r', 
         label= "4 threads")

plt.plot([1000, 10000, 100000, 1000000, 10000000],
         [0.00104099, 0.00177011, 0.00472027, 0.0317005, 0.209461], 'c', 
         label= "6 threads")

plt.plot([1000, 10000, 100000, 1000000, 10000000], 
         [0.00125891, 0.00206225, 0.00511803, 0.0337758, 0.289203], 'm', 
         label= "8 threads")

lgd = plt.legend(bbox_to_anchor=(1.05, 1), loc=2, borderaxespad=0.)

plt.ylabel('Time')
plt.xlabel('Vector size')
plt.xscale('log')

fig.savefig('samplefigure', bbox_extra_artists=(lgd,), bbox_inches='tight')
plt.show()