cmake_minimum_required(VERSION 3.10)
project(OSW-LIB)

set(CMAKE_CXX_STANDARD 11)

# This uses pkg-config, as sdl2-image does not has any CMake bindings (and the sdl2 package via vcpkg is just broken under Linux)
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

add_compile_definitions(FAKE_ARDUINO)

# oswlib
file(GLOB OSWLIB_SRC_FILES ../*.cpp)
add_library(oswLibrary STATIC ${OSWLIB_SRC_FILES})
include_directories(oswLibrary ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
target_link_libraries(oswLibrary PRIVATE ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARIES})

# examples
set(EXAMPLE_INCLUDES ${SDL2_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIRS})
set(EXAMPLE_LIBRARIES ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARIES} oswLibrary)



add_executable(doom-fire doom-fire/main.cpp)
target_include_directories(doom-fire PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(doom-fire PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(fireworks fireworks/main.cpp)
target_include_directories(fireworks PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(fireworks PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(matrix matrix/main.cpp)
target_include_directories(matrix PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(matrix PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(osm osm/main.cpp)
target_include_directories(osm PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(osm PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(perlin perlin/main.cpp)
target_include_directories(perlin PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(perlin PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(rotation rotation/main.cpp)
target_include_directories(rotation PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(rotation PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(shapes shapes/main.cpp)
target_include_directories(shapes PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(shapes PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(text text/main.cpp)
target_include_directories(text PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(text PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(watch-simple watch-simple/main.cpp)
target_include_directories(watch-simple PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(watch-simple PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(water water/main.cpp)
target_include_directories(water PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(water PUBLIC ${EXAMPLE_LIBRARIES})
