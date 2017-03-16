import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt

x_axis=[] #store ids
y_axis=[] #store characters

data=dict();

with open('log.txt','r') as f:
	for line in f:
		x,y,z=line.split(',');
		cal_len=len(z);
		if int(x) in data:
			data[int(x)]+=cal_len
		else:
			data[int(x)]=cal_len
		if int(y) in data:
			pass
		else:
			data[int(y)]=0





x_axis=data.keys();
y_pos=np.arange(len(x_axis));
y_axis=data.values();
w, h = len(x_axis)+1, len(x_axis) +1
table_init = [[0 for x in range(w)] for y in range(h)] 

print table_init
print len(data)
print len(table_init[0])
print len(table_init)
new_file=open('heatmap','w+');
for i in range(0,len(data)):
	table_init[0][i+1]=x_axis[i]
	table_init[i+1][0]=x_axis[i]
	
print table_init
with open('log.txt','r') as f:
	for line in f:
		x,y,z=line.split(',');
		cal_len=len(z);
		row=table_init[0].index(int(x))
		col=table_init[0].index(int(y))
		table_init[row][col]+=cal_len

print table_init
		
for i in range(0,len(table_init)):
	for j in range(0,len(table_init)):
		if (i==0 and j == 0) :
			new_file.write(",")
			continue;
		new_file.write(str(table_init[i][j]))
		if (j!=len(table_init)-1):
			new_file.write(",")
	new_file.write("\n")

new_file.close()


#from http://stackoverflow.com/questions/11617719/how-to-plot-a-very-simple-bar-chart-python-matplotlib-using-input-txt-file

plt.bar(y_pos, y_axis, align='center', alpha=0.5)
plt.xticks(y_pos, x_axis)
plt.ylabel('Word Count')
plt.title('User id')
plt.show();

w, h = len(x_axis), len(x_axis) 
table_plot = [[0 for x in range(w)] for y in range(h)] 


for i in range(1,len(table_init)):
	for j in range(1,len(table_init)):
		table_plot[i-1][j-1]=table_init[i][j]

print table_plot
plt.imshow(table_plot, cmap='rainbow', interpolation='nearest')
plt.show()
