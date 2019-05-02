cmake_minimum_required(VERSION 3.0)

project(roju_tracer)

find_package (Threads)
find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(roju_tracer src/main.cpp)
target_link_libraries(roju_tracer ${SDL2_LIBRARIES})
target_link_libraries(roju_tracer ${CMAKE_THREAD_LIBS_INIT})
