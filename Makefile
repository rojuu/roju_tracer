all: main

main: src/main.cpp
	mkdir -p bin & g++ src/main.cpp -F/Library/Frameworks -framework SDL2 -g -o bin/roju_tracer