cmake_minimum_required(VERSION 3.0)

project(roju_tracer)

add_subdirectory(deps/SDL2-2.0.9)
find_package(Threads REQUIRED)

if (CMAKE_BUILD_TYPE EQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE -Ofast msse3)
endif()

add_executable(IDE_INCLUDES EXCLUDE_FROM_ALL
    src/config.cpp
    src/containers.cpp
    src/HandmadeMath.cpp
    src/hitdetection.cpp
    src/main.cpp
    src/materials.cpp
    src/math.cpp
    src/pcg_extras.hpp
    src/pcg_random.hpp
    src/stb_image_write.cpp
    src/types.cpp
)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} SDL2)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_DIR}/include)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
