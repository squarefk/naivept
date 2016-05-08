main: main.o vec.o model.o triangle.o ray.o sphere.o
	g++ main.o vec.o model.o triangle.o ray.o sphere.o -o main

main.o: main.cpp
	g++ -c main.cpp

vec.o: vec.cpp
	g++ -c vec.cpp

model.o: model.cpp
	g++ -c model.cpp

triangle.o: triangle.cpp
	g++ -c triangle.cpp

ray.o: ray.cpp
	g++ -c ray.cpp

sphere.o: sphere.cpp
	g++ -c sphere.cpp

clean:
	rm *.o