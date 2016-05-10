import math
import numpy

output = open('water.obj','w')

id = 0
a = numpy.zeros((101,102))
for i in range(101):
	for j in range(101):
		x = 0.01 * i
		y = 0.01 * j
		id = id + 1
		a[i][j] = id
		output.write( 'v %.5f %.5f %.5f\n' % (x,y,2.0*math.sin(x*20) ))
	id = id + 1
	a[i][101] = id
	output.write( 'v %.5f %.5f %.5f\n' % (0.01*i,1,-20))


for i in range(100):
	for j in range(101):
		output.write( 'f %d %d %d\n' % (a[i][j],a[i][j+1],a[i+1][j]) )
		output.write( 'f %d %d %d\n' % (a[i+1][j],a[i][j+1],a[i+1][j+1]) )