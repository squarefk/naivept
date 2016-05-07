main: main.o vec.o
	g++ main.o vec.o -o main

main.o: main.cpp
	g++ -c main.cpp

vec.o: vec.cpp
	g++ -c vec.cpp -o vec.o

clean:
	rm *.o