main: main.o vec.o model.o triangle.o ray.o sphere.o hpoint.o grid.o texture.o
	g++ -O2 main.o vec.o model.o triangle.o ray.o sphere.o hpoint.o grid.o texture.o -o main -g
	rm main.o vec.o model.o triangle.o ray.o sphere.o hpoint.o grid.o

main.o: main.cpp
	g++ -O2 -c main.cpp -g

vec.o: vec.cpp
	g++ -O2 -c vec.cpp -g

model.o: model.cpp
	g++ -O2 -c model.cpp -g

triangle.o: triangle.cpp
	g++ -O2 -c triangle.cpp -g

ray.o: ray.cpp
	g++ -O2 -c ray.cpp -g

sphere.o: sphere.cpp
	g++ -O2 -c sphere.cpp -g

hpoint.o: hpoint.cpp
	g++ -O2 -c hpoint.cpp -g

grid.o: grid.cpp
	g++ -O2 -c grid.cpp -g

texture.o: texture.cpp vec.h texture.h
	g++ -O2 -c texture.cpp -g

clean:
	rm *.o