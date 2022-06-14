Import("env")

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