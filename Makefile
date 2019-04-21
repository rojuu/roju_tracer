all: main

main: src/main.cpp
	mkdir -p bin
	g++ -g src/main.cpp -lSDL2 -lpthread -o bin/roju_tracer
