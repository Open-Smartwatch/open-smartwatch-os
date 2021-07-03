import gzip
import os
import re

assPath = os.path.join('include', 'assets', 'www')
wwwPath = os.path.join('src', 'www')

os.makedirs(assPath, exist_ok=True)

for subPath, _, filenames in os.walk(wwwPath):
    for file in filenames:
        # Read the file
        byteFile = open(os.path.join(subPath, file), "rb").read()
        makeGz = file.endswith('.html') or file.endswith('.js') or file.endswith('.css')
        if makeGz:
            byteFile = gzip.compress(byteFile, 9)
        relPath = os.path.relpath(subPath, wwwPath)
        
        # Create new .h string
        fName = file + '.gz' if makeGz else file
        varName = re.sub('[^a-zA-Z0-9]', '_', os.path.join(relPath, fName))
        varName = re.sub('^_*', '', varName)
        fileStr = "const unsigned char " + varName + "[] PROGMEM = {\n"
        for i in range(0, len(byteFile), 12):
            fileStr += '\t'
            subArr = byteFile[i:i+12]
            for j in range(0, len(subArr)):
                fileStr += hex(subArr[j]) + ', '
            fileStr += '\n'
        fileStr = fileStr[:-3] # Strip the last ", \n"
        fileStr += "\n};\nunsigned int " + varName + "_len PROGMEM = " + str(len(byteFile)) + ";"
        
        # Write file to asset path
        outPath = os.path.join(assPath, relPath)
        os.makedirs(outPath, exist_ok=True)
        out = open(os.path.join(outPath, fName + '.h'), "w")
        out.write(fileStr)
        out.close()
