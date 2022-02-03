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
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    link_directories(/usr/local/lib)
endif()
target_link_libraries(oswLibrary PRIVATE ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARIES})

# examples
set(EXAMPLE_INCLUDES ${SDL2_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIRS})
set(EXAMPLE_LIBRARIES ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARIES} oswLibrary)



add_executable(DoomFire doom-fire/main.cpp)
target_include_directories(DoomFire PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(DoomFire PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(Fireworks fireworks/main.cpp)
target_include_directories(Fireworks PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(Fireworks PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(Matrix matrix/main.cpp)
target_include_directories(Matrix PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(Matrix PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(OSM osm/main.cpp)
target_include_directories(OSM PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(OSM PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(Perlin perlin/main.cpp)
target_include_directories(Perlin PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(Perlin PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(Rotation rotation/main.cpp)
target_include_directories(Rotation PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(Rotation PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(Shapes shapes/main.cpp)
target_include_directories(Shapes PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(Shapes PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(Text text/main.cpp)
target_include_directories(Text PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(Text PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(WatchSimple watch-simple/main.cpp)
target_include_directories(WatchSimple PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(WatchSimple PUBLIC ${EXAMPLE_LIBRARIES})

add_executable(Water water/main.cpp)
target_include_directories(Water PUBLIC ${EXAMPLE_INCLUDES})
target_link_libraries(Water PUBLIC ${EXAMPLE_LIBRARIES})
