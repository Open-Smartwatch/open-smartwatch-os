
from shutil import which

Import("env")

if which("ccache"):
    env.Replace(
        CC="ccache arm-none-eabi-gcc",
        CXX="ccache arm-none-eabi-g++",
    )
else:
    env.Replace(
        CC="arm-none-eabi-gcc",
        CXX="arm-none-eabi-g++",
    )
