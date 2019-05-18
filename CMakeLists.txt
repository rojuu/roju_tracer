cmake_minimum_required(VERSION 3.0)

project(roju_tracer)

add_subdirectory(deps/SDL2-2.0.9)

find_package(Threads REQUIRED)

if (CMAKE_BUILD_TYPE EQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE -Ofast msse3)
endif()

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} SDL2)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
