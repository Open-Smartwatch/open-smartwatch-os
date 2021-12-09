import os
Import("env")

buildType = env.GetProjectOption("build_type")

if 'PLATFORMIO_BUILD_TYPE_OVERRIDE' in os.environ:
    # This is utilized by the build everything script of the CI to control the build type without platformio.ini modification
    buildType = os.environ['PLATFORMIO_BUILD_TYPE_OVERRIDE']

if buildType == "debug":
    env.Append(CPPDEFINES=[
        ("DEBUG", "1"), # Just for legacy purposes
        "DEBUG_ESP_CORE"
    ])
    env.Append(CCFLAGS=[
        "-Wall"
    ])
else:
    env.Append(CPPDEFINES=[
        "NDEBUG"
    ])