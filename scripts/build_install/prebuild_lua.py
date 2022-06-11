Import("env")
import subprocess
import sys
import os
import platform

print("Building osw_wrap.cxx with swig..")

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

process = subprocess.Popen([getSwigPath(), '-c++', '-lua', '-I../../include', '-I../../lib/Arduino_GFX', f'-I{getPlatformCore(env)}', "-v", 'osw.i'], cwd='./src/swig')

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