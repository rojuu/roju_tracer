# roju_tracer
A ray tracing thing with the help of [Ray Tracing in a Weekend](http://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf).

## Render result
![alt text](render.png "Render png")

## Compiling
Just compile `src/main.cpp` and link against the SDL2 library. Make sure you have C/C++ runtime libraries on your system as well.

### Linux
You should be able to just use the Makefile on Linux.

### Mac
You should be able to use the Makefile if you have for example installed SDL2 using homebrew.

### Windows
The build.bat file might work on windows, but hasn't been tested in a while. Just point %SDL2% to your SDL2 library folder, or install SDL2 in way your compiler can find it.

## Dependencies
You need to link against these.
- [SDL2](http://libsdl.org/)
- C/C++ runtime libraries

Single-ish header libraries, so doesn't need linking, or any action really. These are all already included in the main.cpp
- [HandmadeMath](https://github.com/HandmadeMath/Handmade-Math)
- [stb_image_write](https://github.com/nothings/stb)
- [PCG Random Number Generator C++ library](https://github.com/imneme/pcg-cpp)