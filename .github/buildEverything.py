#!/usr/lib/python3
import re
import os
import shutil
import logging
import subprocess
logging.basicConfig(level=logging.INFO)

# Configs
includeConfig = os.path.join('include', 'config.h')

# Configure editions
editions = [
    'LIGHT_EDITION_V3_2',
    'LIGHT_EDITION_V3_3',
    'LIGHT_EDITION_V4_0',
    'LIGHT_EDITION_DEV_LUA',
    'GPS_EDITION_V3_1',
    # GPS_EDITION_DEV_ROTATED not, as it is only for testing (right now)
]

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
        # Setup variables
        filename = edition + '-' + lang + '.bin'
        
        # Compile firmware
        logging.info('Compiling ' + filename + '...')
        res = subprocess.run(['pio', 'run', '-e', edition], capture_output=True)
        if res.returncode != 0:
            logging.error('COMPILATION FAILED')
            logging.error(res.stderr.decode())
            exit(2)
            
        # "Export" firmware.bin
        shutil.copy(os.path.join('.pio', 'build', edition, 'firmware.bin'), os.path.join('.', filename))
