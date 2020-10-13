cmake_minimum_required(VERSION 3.10)

# set the project name
set(CMAKE_CXX_STANDARD 11)
project(Shapes)

# add the executable
add_executable(Shapes main.cpp)

find_package(SDL2 CONFIG REQUIRED)
 target_link_libraries(Shapes PRIVATE SDL2::SDL2main SDL2::SDL2-static)

target_include_directories(Shapes PRIVATE ../util/ )