#!/usr/lib/python3
import re
import os
import shutil
import logging
import argparse
import subprocess
logging.basicConfig(level=logging.INFO)

# Configure editions
editions = [
    'LIGHT_EDITION_V3_2',
    'LIGHT_EDITION_V3_3',
    'LIGHT_EDITION_V4_0',
    'LIGHT_EDITION_DEV_LUA',
    'GPS_EDITION_V3_1',
    # GPS_EDITION_DEV_ROTATED not, as it is only for testing (right now)
]

# And compile!
def compile_model(lang):
    # Configs
    includeConfig = os.path.join('include', 'config.h')
    pioConfig = 'platformio.ini'
    
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


if __name__ == "__main__":
	   
    ap = argparse.ArgumentParser()
	
    ap.add_argument("-l", "--support-language", type=str, required=True, help="# model language to compile. (Enter 'all' to compile all language packs.)")
    args = vars(ap.parse_args())

    #if you want all-language packs
    if args["support_language"] == "all":

        # Find all languages
        languages = []
        langIncludePath = os.path.join('include', 'locales')
        for fName in os.listdir(langIncludePath):
            if os.path.join(os.path.join(langIncludePath, fName)):
                languages.append(fName[:-2])  # This drops off the .h

        for lang in languages:
            compile_model(lang)
    else :
        compile_model(args["support_language"])
