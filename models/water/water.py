import math
import numpy

output = open('water.obj','w')

id = 0
a = numpy.zeros((100,101))
for i in range(100):
	for j in range(100):
		x = 0.01 * i
		y = 0.01 * j
		id = id + 1
		a[i][j] = id
		output.write( 'v %.5f %.5f %.5f\n' % (x,y,math.sin(x * 25 + y * 25) + math.sin(x * 37 + 0.01 * i * j) ))
	id = id + 1
	a[i][100] = id
	output.write( 'v %.5f %.5f %.5f\n' % (0.01*i,0,-15))


for i in range(100-1):
	output.write( 'f %d %d %d\n' % (a[i][0],a[i][100],a[i+1][100]) )
	output.write( 'f %d %d %d\n' % (a[i][0],a[i+1][100],a[i+1][0]) )
	for j in range(100-1):
		output.write( 'f %d %d %d\n' % (a[i][j],a[i][j+1],a[i+1][j]) )
		output.write( 'f %d %d %d\n' % (a[i+1][j],a[i][j+1],a[i+1][j+1]) )