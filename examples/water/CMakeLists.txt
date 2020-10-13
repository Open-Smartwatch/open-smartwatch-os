cmake_minimum_required(VERSION 3.10)

# set the project name
set(CMAKE_CXX_STANDARD 11)
project(Example)
find_package(SDL2 CONFIG REQUIRED)

add_compile_definitions(FAKE_ARDUINO)


add_library(simplexNoise STATIC 
    ../../FakeArduino.cpp
    ../../SimplexNoise.cpp
    ../../anim_water_ripple.cpp
    ../../gfx_util.cpp
    )
    
target_link_libraries(simplexNoise PRIVATE SDL2::SDL2main SDL2::SDL2-static)

# add the executable
add_executable(Example main.cpp)

target_link_libraries(Example PRIVATE SDL2::SDL2main SDL2::SDL2-static simplexNoise)

