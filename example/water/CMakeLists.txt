cmake_minimum_required(VERSION 3.10)

# set the project name
set(CMAKE_CXX_STANDARD 11)
project(Example)

# add the simplex library
add_library(simplexNoise STATIC ../../src/math/perlin/SimplexNoise.cpp)

# add the executable
add_executable(Example main.cpp)

find_package(SDL2 CONFIG REQUIRED)
 target_link_libraries(Example PRIVATE SDL2::SDL2main SDL2::SDL2-static simplexNoise)

target_include_directories(Example PRIVATE ../util/ )