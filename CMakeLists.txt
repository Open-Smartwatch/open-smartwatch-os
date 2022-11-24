cmake_minimum_required (VERSION 3.10)
project (OSW-OS-Emulator)

set(CMAKE_CXX_STANDARD 20)

# Prepare some defines, which are normally evaluated using some Python snippets...
execute_process(COMMAND "git" "rev-parse" "--short" "HEAD" OUTPUT_VARIABLE GIT_COMMIT_HASH)
execute_process(COMMAND "git" "log" "-1" "--pretty=format:%cd" "--date=format:%Y-%m-%dT%H:%M:%S%z" OUTPUT_VARIABLE GIT_COMMIT_TIME)
execute_process(COMMAND "git" "rev-parse" "--abbrev-ref" "HEAD" OUTPUT_VARIABLE GIT_BRANCH_NAME)
string(STRIP ${GIT_COMMIT_HASH} GIT_COMMIT_HASH)
string(STRIP ${GIT_COMMIT_TIME} GIT_COMMIT_TIME)
string(STRIP ${GIT_BRANCH_NAME} GIT_BRANCH_NAME)

# For threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# This uses pkg-config, as sdl2-image does not has any CMake bindings (and the sdl2 package via vcpkg is just broken under Linux)
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2>=2.0.20)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)

# Pngle
file(GLOB_RECURSE SOURCES_Pngle ./lib/pngle/*.c)
add_library(Pngle ${SOURCES_Pngle})
target_include_directories(Pngle PUBLIC ./lib/pngle/src/)

# ArduinoJSON
add_subdirectory(emulator/lib/ArduinoJson)

# Jzon
add_library(Jzon emulator/lib/Jzon/Jzon.cpp)
target_include_directories(Jzon PUBLIC emulator/lib/Jzon/)

# cmdline
add_library(cmdline INTERFACE)
target_include_directories(cmdline INTERFACE emulator/lib/cmdline/)

# utest
add_library(utest INTERFACE)
target_include_directories(utest INTERFACE emulator/lib/utest/)

# ImGUI
add_library(ImGUI
    emulator/lib/ImGUI/imgui.cpp
    emulator/lib/ImGUI/imgui_draw.cpp
    emulator/lib/ImGUI/imgui_widgets.cpp
    emulator/lib/ImGUI/imgui_tables.cpp
    emulator/lib/ImGUI/backends/imgui_impl_sdl.cpp
    emulator/lib/ImGUI/backends/imgui_impl_sdlrenderer.cpp
)
target_include_directories(ImGUI PUBLIC
    emulator/lib/ImGUI/
    emulator/lib/ImGUI/backends
    ${SDL2_INCLUDE_DIRS}
)
target_link_libraries(ImGUI LINK_PUBLIC
    ${SDL2_LIBRARY}
    ${SDL2IMAGE_LIBRARIES}
)

# Emulator
file(GLOB_RECURSE SOURCES_OSW ./src/*.cpp)
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
    ${SDL2_INCLUDE_DIRS}
    ${SDL2IMAGE_INCLUDE_DIRS}
)
target_link_libraries(emulator.run LINK_PUBLIC
    Pngle
    ArduinoJson
    Threads::Threads
    Jzon
    ${SDL2_LIBRARY}
    ${SDL2IMAGE_LIBRARIES}
    ImGUI
    cmdline
    utest
)
target_compile_definitions(emulator.run PUBLIC
    OSW_TARGET_PLATFORM_HEADER="platform/EMULATOR.h"
    OSW_EMULATOR=1
    GIT_COMMIT_HASH="${GIT_COMMIT_HASH}"
    GIT_COMMIT_TIME="${GIT_COMMIT_TIME}"
    GIT_BRANCH_NAME="${GIT_BRANCH_NAME}"
    PIO_ENV_NAME="VIRTUAL"
    $<$<CONFIG:Debug>:
      DEBUG=1 # Just for legacy reasons
    >
    $<$<CONFIG:Release>:
      NDEBUG=1
    >
    # Comment these as you wish...
    OSW_FEATURE_STATS_STEPS
    GAME_SNAKE=1
    GAME_BRICK_BREAKER=1
)
target_compile_options(emulator.run PUBLIC
  $<$<CONFIG:Debug>:
    -O0
    -g3
    -Wall
  >
  $<$<CONFIG:Release>:
    -O4
  >
)

# Including SDL2 using the system libraries is broken under MacOS, so we have to add this path manually...
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  link_directories(/usr/local/lib)
endif()

# And add unit testing, as we broke our stuff often enough...
enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
add_test(NAME emulator.run COMMAND emulator.run --unit_tests)