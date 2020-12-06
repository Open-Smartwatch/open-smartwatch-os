cmake_minimum_required(VERSION 3.10)

# set the project name
set(CMAKE_CXX_STANDARD 11)
project(Example)
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)

add_compile_definitions(FAKE_ARDUINO)

file(GLOB SRC_FILES ../../*.cpp)

add_library(oswLibrary STATIC 
    ${SRC_FILES}
    )
target_link_libraries(oswLibrary PRIVATE SDL2::SDL2main SDL2::SDL2-static SDL2::SDL2_image)

# add the executable
add_executable(Example main.cpp)

target_link_libraries(Example PRIVATE SDL2::SDL2main SDL2::SDL2-static SDL2::SDL2_image oswLibrary)

