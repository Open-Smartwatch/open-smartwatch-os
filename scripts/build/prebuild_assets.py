#! /usr/bin/env python3

import io
import os
import re
import sys
import gzip
import hashlib
import argparse
try:
    from PIL import Image
except ImportError:
    # According to https://docs.platformio.org/en/stable/scripting/examples/extra_python_packages.html, this may help:
    Import("env")
    env.Execute("$PYTHONEXE -m pip install pillow")
    from PIL import Image

def createAssets(srcPath, assPath, convertAssetToSourceCode, force):
    os.makedirs(assPath, exist_ok=True)

    # Get all paths of the src folder
    srcFiles = {}
    for subPath, _, filenames in os.walk(srcPath):
        for file in filenames:
            path = os.path.join(subPath, file)
            srcFiles[os.path.relpath(path, srcPath)] = os.path.getmtime(path)

    # Clear the assets folder as needed
    for subPath, _, filenames in os.walk(assPath):
        for file in filenames:
            # fileStripped = file.removesuffix('.h').removesuffix('.gz')
            # the above line is only available in python 3.9, so we use the following instead:
            fileStripped = file
            if (split := os.path.splitext(fileStripped))[1] == '.h':
                fileStripped = split[0]
            if (split := os.path.splitext(fileStripped))[1] == '.gz':
                fileStripped = split[0]
            assFile = os.path.join(subPath, fileStripped)
            path = os.path.relpath(assFile, assPath)
            if path not in srcFiles:
                os.remove(os.path.join(subPath, file))
                print('Removed: ' + assFile)

    # Create the assets from the source codes
    for subPath, mtime in srcFiles.items():
        srcFile = os.path.join(srcPath, subPath)

        makeGz = subPath.endswith('.html') or subPath.endswith('.js') or subPath.endswith('.css')
        assFile = os.path.join(assPath, subPath)
        if makeGz:
            assFile = assFile + '.gz' # Add .gz to the end of the file as needed
        assFileHeader = assFile + '.h'

        # Check if the file needs to be updated
        if not force and os.path.exists(assFileHeader) and os.path.getmtime(assFileHeader) >= mtime:
            print('Skipped: ' + assFileHeader)
            continue

        # Read the file
        fileStr = convertAssetToSourceCode(srcFile, subPath + ('.gz' if makeGz else ''))
        if fileStr is None:
            print('Ignored: ' + subPath)
            continue
            
        # Write file to asset path
        os.makedirs(os.path.split(assFileHeader)[0], exist_ok=True)
        with open(assFileHeader, 'w') as out:
            out.write(fileStr)
            out.close()
        print('Updated: ' + assFileHeader)

def makeGzStr(srcPath, subPath):
    with open(srcPath, 'rb') as f:
        byteFile = f.read()
    if subPath.endswith('.gz'):
        byteFile = gzip.compress(byteFile, 9)
        
    # Create new .h string
    varName = re.sub('[^a-zA-Z0-9]', '_', subPath) # Replace all non-alphanumeric characters with underscores
    varName = re.sub('^_*', '', varName)
    fileStr = "#pragma once\nconst char " + varName + "[] PROGMEM = {\n"
    for i in range(0, len(byteFile), 12):
        fileStr += '\t'
        subArr = byteFile[i:i+12]
        for j in range(0, len(subArr)):
            fileStr += hex(subArr[j]) + ', '
        fileStr += '\n'
    fileStr = fileStr[:-3] # Strip the last ", \n"
    fileStr += "\n};\nconst unsigned long " + varName + "_len PROGMEM = " + str(len(byteFile)) + ";"
    return fileStr

def makeIconImgStr(srcPath, subPath):
    if not subPath.endswith('.png'):
        return None

    try:
        img = Image.open(srcPath, 'r')
    except:
        print(f'!!! The build process noticed that the image {srcPath} is not a valid image. Please make sure it is in an valid image format - this is commonly caused by cloning this repository without Git LFS enabled.')
        raise
    assert img.size[0] == img.size[1], 'Image must be square'
    assert img.size[0] % 8 == 0, 'Image dimension must be a multiple of 8'
    imgData = list(img.getdata())

    byteFile = []
    averageColor = (0, 0, 0)
    for i in range(0, len(imgData), 8):
        b = 0b00000000
        for j in range(0, 8):
            if imgData[i + j] != (0, 0, 0):
                b = b | (1 << j)
                averageColor = tuple(map(lambda x, y: (x + y) // 2, averageColor, imgData[i + j]))
        byteFile.append(b)
        
    # Create new .h string
    rawDataName = '_' + hashlib.md5(str(byteFile).encode()).hexdigest() # using a random name, so it is not accessible, but still statically embedded
    fileStr = "#pragma once\n#include <icon/OswIconProgmem.h>\nconst unsigned char " + rawDataName + "[] PROGMEM = {\n"
    for i in range(0, len(byteFile), 12):
        fileStr += '\t'
        subArr = byteFile[i:i+12]
        for j in range(0, len(subArr)):
            fileStr += hex(subArr[j]) + ', '
        fileStr += '\n'
    fileStr = fileStr[:-3] # Strip the last ", \n"
    fileStr += "\n};"
    varName = re.sub('[^a-zA-Z0-9]', '_', subPath) # Replace all non-alphanumeric characters with underscores
    varName = re.sub('^_*', '', varName)
    fileStr += "\nstatic const OswIconProgmem " + varName + "{" + f"{rawDataName}, {img.size[0]}, rgb565({averageColor[0]}, {averageColor[1]}, {averageColor[2]})" + "};"
    return fileStr

def makePngImgStr(srcPath, subPath):
    if not subPath.endswith('.png'):
        return None

    img = Image.open(srcPath, 'r')
    
    byts = io.BytesIO()
    img.save(byts, format='PNG', optimize=True, compress_level=9)
    byteFile = byts.getvalue()
        
    # Create new .h string
    varName = re.sub('[^a-zA-Z0-9]', '_', subPath) # Replace all non-alphanumeric characters with underscores
    varName = re.sub('^_*', '', varName)
    fileStr = "#pragma once\nconst unsigned char " + varName + "[] PROGMEM = {\n"
    for i in range(0, len(byteFile), 12):
        fileStr += '\t'
        subArr = byteFile[i:i+12]
        for j in range(0, len(subArr)):
            fileStr += hex(subArr[j]) + ', '
        fileStr += '\n'
    fileStr = fileStr[:-3] # Strip the last ", \n"
    fileStr += "\n};\nconst unsigned int " + varName + "_length PROGMEM = " + str(len(byteFile)) + ";"
    fileStr += "\nconst unsigned short " + varName + "_width PROGMEM = " + str(img.size[0]) + ";"
    fileStr += "\nconst unsigned short " + varName + "_height PROGMEM = " + str(img.size[1]) + ";"
    return fileStr

parser = argparse.ArgumentParser()
parser.add_argument('--output-asset-path', default=os.path.join('include', 'assets'), help='The path to the assets folder')
args, _ = parser.parse_known_args()

useForce = "--force" in sys.argv
createAssets(os.path.join('lib', 'open-smartwatch-web', 'dist', 'open-smartwatch-web'), os.path.join(args.output_asset_path, 'www'), makeGzStr, useForce)
createAssets(os.path.join('img', 'icons'), os.path.join(args.output_asset_path, 'img', 'icons'), makeIconImgStr, useForce)
createAssets(os.path.join('img', 'static'), os.path.join(args.output_asset_path, 'img', 'static'), makePngImgStr, useForce)
