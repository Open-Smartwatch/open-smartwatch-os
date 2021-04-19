Import("env")
import subprocess
import sys
import os

print("Building osw_wrap.cxx with swig..")

# Find the framework Core includes so we can use Print.h
def getPlatformCore(env):
    packagesDir = env['PROJECT_PACKAGES_DIR']
    framework = env['PIOFRAMEWORK'][0]
    platform = env['PIOPLATFORM']
    board = env['BOARD_MCU']
    
    return os.path.join(os.path.join(os.path.join(packagesDir, f"framework-{framework}{platform}"), "cores"), board)

process = subprocess.Popen(['swig', '-c++', '-lua', '-I../../include', '-I../../lib/Arduino_GFX', f'-I{getPlatformCore(env)}', "-v", 'osw.i'], cwd=r'./src/swig')

process.wait()
return_code = process.poll()
if return_code != 0:
    print("Building osw_wrap.cxx with swig failed with return code:")
    print(return_code)
    sys.exit(99)
else:
    print("Building osw_wrap.cxx with swig success")