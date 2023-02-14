import gzip
import os
import re

assPath = os.path.join('include', 'assets', 'www')
wwwPath = os.path.join('src', 'www')

os.makedirs(assPath, exist_ok=True)

# Get all paths of the www folder
wwwFiles = {}
for subPath, _, filenames in os.walk(wwwPath):
    for file in filenames:
        path = os.path.join(subPath, file)
        wwwFiles[os.path.relpath(path, wwwPath)] = os.path.getmtime(path)

# Clear the assets folder as needed
for subPath, _, filenames in os.walk(assPath):
    for file in filenames:
        fileStripped = file.removesuffix('.h').removesuffix('.gz')
        assFile = os.path.join(subPath, fileStripped)
        path = os.path.relpath(assFile, assPath)
        if path not in wwwFiles:
            os.remove(os.path.join(subPath, file))
            print('Removed: ' + assFile)

# Create the assets from the source codes
for subPath, mtime in wwwFiles.items():
    wwwFile = os.path.join(wwwPath, subPath)

    makeGz = subPath.endswith('.html') or subPath.endswith('.js') or subPath.endswith('.css')
    assFile = os.path.join(assPath, subPath)
    assFile = assFile + '.gz' if makeGz else assFile # Add .gz to the end of the file as needed
    assFileHeader = assFile + '.h'

    # Check if the file needs to be updated
    if os.path.exists(assFileHeader) and os.path.getmtime(assFileHeader) >= mtime:
        print('Skipped: ' + assFileHeader)
        continue

    # Read the file
    with open(wwwFile, 'rb') as f:
        byteFile = f.read()
    if makeGz:
        byteFile = gzip.compress(byteFile, 9)
        
    # Create new .h string
    varName = re.sub('[^a-zA-Z0-9]', '_', subPath + ('.gz' if makeGz else '')) # Replace all non-alphanumeric characters with underscores
    varName = re.sub('^_*', '', varName)
    fileStr = "const char " + varName + "[] PROGMEM = {\n"
    for i in range(0, len(byteFile), 12):
        fileStr += '\t'
        subArr = byteFile[i:i+12]
        for j in range(0, len(subArr)):
            fileStr += hex(subArr[j]) + ', '
        fileStr += '\n'
    fileStr = fileStr[:-3] # Strip the last ", \n"
    fileStr += "\n};\nunsigned long " + varName + "_len PROGMEM = " + str(len(byteFile)) + ";"
        
    # Write file to asset path
    os.makedirs(os.path.split(assFileHeader)[0], exist_ok=True)
    with open(assFileHeader, 'w') as out:
        out.write(fileStr)
        out.close()
    print('Updated: ' + assFileHeader)
