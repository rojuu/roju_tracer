all: main

main: src/main.cpp
	mkdir -p bin
	clang++ -Ofast -msse3 -std=c++11 -g src/main.cpp -lSDL2 -lpthread -o bin/roju_tracer
