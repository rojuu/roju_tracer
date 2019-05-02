cmake_minimum_required(VERSION 3.0)

project(roju_tracer)

add_subdirectory(deps/SDL2-2.0.9)

find_package(Threads REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(roju_tracer src/main.cpp)
target_link_libraries(roju_tracer SDL2)
target_link_libraries(roju_tracer ${CMAKE_THREAD_LIBS_INIT})
