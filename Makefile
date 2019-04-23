all: main

main: src/main.cpp
	mkdir -p bin
	g++ -g src/main.cpp -O3 -msse3 -lSDL2 -lpthread -o bin/roju_tracer
