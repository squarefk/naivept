import math
import numpy

outoutfilename = open('water.obj', 'w')
id = 0
a = numpy.zeros((101,102))
for i in range(101):
	for j in range(101):
		x = 0.01 * i
		y = 0.01 * j
		id = id + 1
		a[i][j] = id
		z = 0
		for k in range(5):
			z += 0.5 ** k * (
				math.sin(((x+12)*13+3*(y+15))*k+98)+
				math.sin(((y+87)*9+4*(x+465))*k+41)+
				math.sin(k*((x+45)*10+(y+1564)*7)+16502)+
				math.sin(k*((x+84)*12+(y+5874)*2)+17))*2
		outoutfilename.write( 'v %.5f %.5f %.5f\n' % (100*x,50*y,z))
	id = id + 1
	a[i][101] = id
	outoutfilename.write( 'v %.5f %.5f %.5f\n' % (100*x,0.0,-50.0))


for i in range(100):
	for j in range(100):
		outoutfilename.write( 'f %d %d %d\n' % (a[i][j],a[i+1][j],a[i][j+1]) )
		outoutfilename.write( 'f %d %d %d\n' % (a[i][j+1],a[i+1][j],a[i+1][j+1]) )
	outoutfilename.write( 'f %d %d %d\n' % (a[i][101],a[i+1][101],a[i][0]) )
	outoutfilename.write( 'f %d %d %d\n' % (a[i][0],a[i+1][101],a[i+1][0]) )

outoutfilename.close()