main: main.o vec.o model.o triangle.o ray.o sphere.o hpoint.o grid.o texture.o
	g++ -O2 main.o vec.o model.o triangle.o ray.o sphere.o hpoint.o grid.o texture.o -o main
	rm *.o

main.o: main.cpp
	g++ -O2 -c main.cpp

vec.o: vec.cpp
	g++ -O2 -c vec.cpp

model.o: model.cpp
	g++ -O2 -c model.cpp

triangle.o: triangle.cpp
	g++ -O2 -c triangle.cpp

ray.o: ray.cpp
	g++ -O2 -c ray.cpp

sphere.o: sphere.cpp
	g++ -O2 -c sphere.cpp

hpoint.o: hpoint.cpp
	g++ -O2 -c hpoint.cpp

grid.o: grid.cpp
	g++ -O2 -c grid.cpp

texture.o: texture.cpp
	g++ -O2 -c texture.cpp

clean:
	rm *.o