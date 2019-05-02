# roju_tracer
A ray tracing thing with the help of [Ray Tracing in a Weekend](http://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf).

## Render result
![alt text](render.png "Render png")

## Compiling
This project uses [CMake](https://cmake.org/) for generating projects or makefiles for each platform. Currently only tested on Linux (Manjaro), but should in theory work on Windows and OSX as well.

## Dependencies
- [SDL2](http://libsdl.org/) cmake will automatically build this from source (included in deps folder)
- C/C++ runtime libraries

Following are single-ish header libraries, so they don't require any specific actions really. These are all already included in the main.cpp
- [HandmadeMath](https://github.com/HandmadeMath/Handmade-Math)
- [stb_image_write](https://github.com/nothings/stb)
- [PCG Random Number Generator C++ library](https://github.com/imneme/pcg-cpp)

