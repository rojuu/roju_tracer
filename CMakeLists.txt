cmake_minimum_required(VERSION 3.0)

project(roju_tracer)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(SDL2_DIR deps/SDL2-2.0.20)
add_subdirectory(${SDL2_DIR})

find_package(Threads REQUIRED)

include_directories(src)
add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_DIR}/include)
