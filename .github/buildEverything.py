#!/usr/lib/python3
import re
import os
import shutil
import logging
import subprocess
logging.basicConfig(level=logging.INFO)

# Configs
includeConfig = os.path.join('include', 'config.h')
pioConfig = 'platformio.ini'

# Configure editions
editions = []
build_models = os.popen("cat platformio.ini | grep '\[\env:.*\]\'").read()
build_models = build_models.strip().split("\n")
for build_model in build_models:
    editions.append(build_model.replace("[env:", "").replace("]", ""))

# Find all languages
languages = []
langIncludePath = os.path.join('include', 'locales')
for fName in os.listdir(langIncludePath):
    if os.path.join(os.path.join(langIncludePath, fName)):
        languages.append(fName[:-2]) # This drops off the .h

# And compile!
for lang in languages:
    # Modify configs language
    logging.info('Setting language to ' + lang + '...')
    configIn = open(includeConfig, 'r')
    configStr = configIn.read()
    configIn.close()
    configStr, hitCount = re.subn('("locales/).+(\.h")', r'\1' + lang + r'\2', configStr)
    if hitCount != 1:
        logging.error('Error on setting language!')
        exit(1)
    configOut = open(includeConfig, 'w')
    configOut.write(configStr)
    configOut.close()
    
    # Always clean after changing the language (just-in-case)
    logging.info('Cleanup...')
    subprocess.run(['pio', 'run', '-t', 'clean'], capture_output=True)
    for edition in editions:
        filename = edition + '-' + lang + '.bin'
        try:
            os.remove(os.path.join('.', filename))
            logging.info('Deleted ' + filename)
        except OSError:
            pass

    # Compile editions
    for edition in editions:
        def doBuild(makeDebug):
            # Setup variables
            filename = edition + '-' + lang + ('-debug' if makeDebug else '') + '.bin'
            
            # Setup build type (using the config file via replacing, as platformio does not allow setting the property using Python)
            configIn = open(pioConfig, 'r')
            configStr = configIn.read()
            configIn.close()
            configStr, hitCount = re.subn('(build_type\s?=\s?)(\w+)', r'\1' + ('debug' if makeDebug else 'release'), configStr)
            if hitCount == 0:
                logging.error('Error on setting build type!')
                exit(4)
            configOut = open(pioConfig, 'w')
            configOut.write(configStr)
            configOut.close()

            # Compile firmware
            logging.info('Compiling ' + filename + '...')
            try:
                res = subprocess.run(['pio', 'run', '-e', edition], capture_output=True)
            except KeyboardInterrupt:
                exit(3)
            if res.returncode != 0:
                logging.error('COMPILATION FAILED')
                logging.error(res.stderr.decode())
                exit(2)
                
            # "Export" firmware.bin
            shutil.copy(os.path.join('.pio', 'build', edition, 'firmware.bin'), os.path.join('.', filename))
        doBuild(True)
        doBuild(False)
