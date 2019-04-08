# roju_tracer
A ray tracing thing with the help of [Ray Tracing in a Weekend](http://www.realtimerendering.com/raytracing/Ray%20Tracing%20in%20a%20Weekend.pdf).

## Render result
![alt text](render.png "Logo Title Text 1")

## Compiling
Just compile `src/main.cpp` and link against the SDL2 library. Make sure you have CRT on your system as well. You should be able to just use the Makefile on Linux.

## Dependencies
You need to link against these.
- [SDL2](http://libsdl.org/)
- C++/C runtime libraries

Single header libraries, so doesn't need linking.
- [HandmadeMath](https://github.com/HandmadeMath/Handmade-Math)
- [stb_image_write](https://github.com/nothings/stb)