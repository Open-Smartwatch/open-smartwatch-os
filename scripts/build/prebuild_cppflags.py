Import("env")

env.Append(CXXFLAGS=[
    "-std=gnu++2a", # The ESPs toolchain uses some gnu-specific extensions, so we need to use gnu-flavour of the standard
    "-fconcepts"
])

if env.GetBuildType() == "debug":
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