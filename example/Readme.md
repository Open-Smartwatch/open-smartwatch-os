# Requirements

The test code, to run the stuff on a desktop is a bit messy, but it works.

## Getting Started

### Dependencies

Install *cmake*

    brew install cmake

Install *vcpkg*
See: https://docs.microsoft.com/en-us/cpp/build/vcpkg?view=vs-2019

    git clone https://github.com/microsoft/vcpkg.git
    
Assuming you are running macOs or linux:

    bash bootstrap-vcpkg.sh
    ./vcpkg install sdl2
    
There will be an output like this:

    CMake projects should use: "-DCMAKE_TOOLCHAIN_FILE=/home/you/.../vcpkg/scripts/buildsystems/vcpkg.cmake"

Copy the parameter, for the steps below.

### Running an example

    cd gfx-test
    mkdir build
    cd build
    cmake .. -DCMAKE_TOOLCHAIN_FILE=/home/you/.../vcpkg/scripts/buildsystems/vcpkg.cmake
    make
    ./GfxTest