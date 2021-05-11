import requests
import os
import tarfile
import platform

SWIG_LINUX_VERSION = '4.0.1-5build1'
SWIG_URL = "https://anaconda.org/anaconda/swig/4.0.2/download/win-64/swig-4.0.2-hd77b12b_3.tar.bz2"
SWIG_PATH = "bin/swig"
SWIG_TAR = "swig.tar.bz2"
SWIG_TAR_PATH = os.path.join(SWIG_PATH, SWIG_TAR)

def installSwigWindows():
    print("Create directory")
    os.makedirs(SWIG_PATH, exist_ok=True)

    print("Downloading SWIG...")
    res = requests.get(SWIG_URL)
    with open(SWIG_TAR_PATH, 'wb') as outfile:
        outfile.write(res.content)

    print("Extracting...")
    tar = tarfile.open(SWIG_TAR_PATH, "r:bz2")
    tar.extractall(SWIG_PATH)
    tar.close()

    print("Cleaning up...")
    os.unlink(SWIG_TAR_PATH)

def installSwigLinux():
    os.system('sudo apt-get update')
    os.system(f'sudo apt-get install swig={SWIG_LINUX_VERSION}')

if (platform.system() == "Windows"):
    installSwigWindows()
elif (platform.system() == "Linux"):
    installSwigLinux()
else:
    print(f"{platform.system()} not supported")

print("Done")