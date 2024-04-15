Import("env")
import sys

# Only build the swig file when the feature is enabled
buildWantsLua = False
for flag in env['BUILD_FLAGS']:
    if 'OSW_FEATURE_LUA' in flag:
        buildWantsLua = True
        break
else:
    buildWantsLua = 'OSW_FEATURE_LUA' in env['CPPDEFINES']

if buildWantsLua:
    import os
    import platform
    import subprocess
    print("Building osw_wrap.cxx with swig...")

    # Find the framework Core includes so we can use Print.h
    def getPlatformCore(env):
        packagesDir = env['PROJECT_PACKAGES_DIR']
        framework = env['PIOFRAMEWORK'][0]
        platform = env['PIOPLATFORM']
        board = env['BOARD_MCU']
        
        return os.path.join(os.path.join(os.path.join(packagesDir, f"framework-{framework}{platform}"), "cores"), board)

    def getSwigPath():
        path = "swig"
        
        #Attempt to use swig.exe from install.py
        windowsPath = os.path.join("bin", "swig")
        exe = "swig.exe"
        if (platform.system() == "Windows"):
            if (os.path.exists(windowsPath)):
                #Search for swig.exe
                for root, subdirs, files in os.walk(windowsPath):
                    if (exe in files):
                        return os.path.join(root, exe)
        
        return path

    # Try to pass on the build flags to swig
    buildFlags = []
    for flag in env['BUILD_FLAGS']:
        if flag.startswith('-D'):
            buildFlags.append(flag)
    for flag in env['CPPDEFINES']:
        if isinstance(flag, str):
            buildFlags.append(f'-D{flag}')
        else:
            buildFlags.append(f'-D{flag[0]}={flag[1]}')
    print(f'Build flags: {buildFlags}')

    process = subprocess.Popen([getSwigPath(), '-c++', '-lua', '-I../../include', '-I../../lib/Arduino_GFX', *buildFlags, f'-I{getPlatformCore(env)}', '-v', 'osw.i'], cwd='./src/swig')

    process.wait()
    return_code = process.poll()
    if return_code != 0:
        print("Building osw_wrap.cxx with swig failed with return code:")
        print(return_code)
        sys.exit(99)
    else:
        print("Building osw_wrap.cxx with swig successful")

    # Add define checks to disable the cxx file when it is not enabled to be build
    with open('./src/swig/osw_wrap.cxx', 'r') as original: data = original.read()
    with open('./src/swig/osw_wrap.cxx', 'w') as modified: modified.write("#ifdef OSW_FEATURE_LUA\n" + data + "\n#endif")

    # And compile luac for compressing lua chunks
    print("Building luac ...")
    ##    return_code = os.system("cd ./lib/LUA/; gcc -O2 -std=c89 -DLUA_USE_C89 -DMAKE_LUAC -o ../../build/luac onelua.c -lm")
    return_code = os.system("cd ./lib/LUA/; make luac; cp luac ../../build/luac")
    if return_code != 0:
        print("Building lua compiler (luac) failed with return code:")
        print(return_code)
        sys.exit(99)
    else:
        print("Building lua compiler (luac) successful")

    print("Compiling lua bytecode with luac ...")
    os.system("cd src.lua; for i in $(find . -iname '*.lua'); do echo compiling $i; ../build/luac -s -o ../data/lua/$i $i; done")

else:
    print("Skipping building osw_wrap.cxx with swig because OSW_FEATURE_LUA is not defined")
