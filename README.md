# roju_tracer
A ray tracing thing with the help of [Ray Tracing in a Weekend](http://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf).

## Render result
![alt text](render.png "Render png")

## Compiling
This project uses [CMake](https://cmake.org/) for generating projects or makefiles for each platform. Currently only tested on Linux (Manjaro), but should in theory work on Windows and OSX as well.

## Dependencies
You need to link against these.
- [SDL2](http://libsdl.org/)
- C/C++ runtime libraries

Single-ish header libraries, so doesn't need linking, or any action really. These are all already included in the main.cpp
- [HandmadeMath](https://github.com/HandmadeMath/Handmade-Math)
- [stb_image_write](https://github.com/nothings/stb)
- [PCG Random Number Generator C++ library](https://github.com/imneme/pcg-cpp)

