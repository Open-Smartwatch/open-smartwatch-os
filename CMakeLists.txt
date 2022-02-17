cmake_minimum_required (VERSION 2.8.11)
project (OSW-OS-Emulator)

set(CMAKE_CXX_STANDARD 20)

# For threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# This uses pkg-config, as sdl2-image does not has any CMake bindings (and the sdl2 package via vcpkg is just broken under Linux)
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

# Pngle
file(GLOB_RECURSE SOURCES_Pngle ./lib/pngle/*.c)
add_library(Pngle ${SOURCES_Pngle})
target_include_directories(Pngle PUBLIC ./lib/pngle/)

# oswLib
file(GLOB_RECURSE SOURCES_OSW_LIB ./lib/lib-open-smartwatch/*.cpp)
add_library(oswLib ${SOURCES_OSW_LIB})
target_link_libraries(oswLib LINK_PUBLIC
    ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARIES}
)
target_include_directories(oswLib PUBLIC
    ./lib/lib-open-smartwatch
    ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS}
)
target_compile_definitions(oswLib PUBLIC
    FAKE_ARDUINO=1
)

# ArduinoJSON
add_subdirectory(emulator/lib/ArduinoJson)

# Jzon
add_library(Jzon emulator/lib/Jzon/Jzon.cpp)
target_include_directories(Jzon PUBLIC emulator/lib/Jzon/)

# Emulator
file(GLOB_RECURSE SOURCES_OSW ./src/*.cpp)
list(FILTER SOURCES_OSW EXCLUDE REGEX ".*/src/devices/bma400\\.cpp$")
list(FILTER SOURCES_OSW EXCLUDE REGEX ".*/src/devices/bme280\\.cpp$")
list(FILTER SOURCES_OSW EXCLUDE REGEX ".*/src/devices/ds3231mz\\.cpp$")
list(FILTER SOURCES_OSW EXCLUDE REGEX ".*/src/devices/qmc5883l\\.cpp$")
list(FILTER SOURCES_OSW EXCLUDE REGEX ".*/src/hal/Arduino_Canvas_Graphics2D\\.cpp$")
list(FILTER SOURCES_OSW EXCLUDE REGEX ".*/src/hal/esp32/sd_filesystem\\.cpp$")
list(FILTER SOURCES_OSW EXCLUDE REGEX ".*/src/hal/esp32/spiffs_filesystem\\.cpp$")
list(FILTER SOURCES_OSW EXCLUDE REGEX ".*/src/hal/gps\\.cpp$")
list(FILTER SOURCES_OSW EXCLUDE REGEX ".*/src/hal/sd\\.cpp$")
list(FILTER SOURCES_OSW EXCLUDE REGEX ".*/src/services/OswServiceTaskBLECompanion\\.cpp$")
list(FILTER SOURCES_OSW EXCLUDE REGEX ".*/src/services/OswServiceTaskMemMonitor\\.cpp$")
file(GLOB_RECURSE SOURCES_OSW_EMULATOR ./emulator/src/*.cpp)
add_executable(emulator.run
    ${SOURCES_OSW}
    ${SOURCES_OSW_EMULATOR}
)
target_include_directories(emulator.run PUBLIC
    ./emulator/include
    ./include
    ./lib/lib-open-smartwatch
    emulator/lib/Jzon/
)
target_link_libraries(emulator.run LINK_PUBLIC
    oswLib
    Pngle
    ArduinoJson
    Threads::Threads
    Jzon
)
target_compile_definitions(emulator.run PUBLIC
    OSW_TARGET_PLATFORM_HEADER="platform/EMULATOR.h"
    FAKE_ARDUINO=1
)