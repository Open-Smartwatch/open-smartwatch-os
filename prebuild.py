Import("env")
import subprocess
import sys

print("Building osw_wrap.cxx with swig..")

process = subprocess.Popen(['swig', '-c++', '-lua', '-I../../include', '-I../../lib/Arduino_GFX', 'osw.i'], cwd=r'./src/swig')

process.wait()
return_code = process.poll()
if return_code != 0:
    print("Building osw_wrap.cxx with swig failed with return code:")
    print(return_code)
    sys.exit(99)
else:
    print("Building osw_wrap.cxx with swig success")