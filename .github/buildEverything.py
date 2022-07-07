#!/usr/lib/python3
import re
import os
import shutil
import logging
import argparse
import subprocess
logging.basicConfig(level=logging.INFO)

pioConfig = 'platformio.ini'

# Configure editions
editions = []
fh = open(pioConfig, 'r')
for line in fh.readlines():
    if line.startswith('[env:'):
        editions.append(line.strip()[5:-1])
fh.close()

# Find all languages
languages = []
langIncludePath = os.path.join('include', 'locales')
for fName in os.listdir(langIncludePath):
    if os.path.join(os.path.join(langIncludePath, fName)):
        languages.append(fName[:-2])  # This drops off the .h

# And compile!
def compile_model(lang, edition):
    # Configs
    includeConfig = os.path.join('include', 'config.h')
    
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

    filename = edition + '-' + lang + '.bin'
    try:
        os.remove(os.path.join('.', filename))
        logging.info('Deleted ' + filename)
    except OSError:
        pass

    # Compile editions
    def doBuild(makeDebug):
        # Setup variables
        filename = (str(args["support_communication"] + '-') if str(args["support_communication"]) != "" else '') + edition + '-' + lang + ('-debug' if makeDebug else '') + '.bin'
        
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
    ap.add_argument("-m", "--support-model", type=str, required=True, help="# model type to compile. (Enter 'all' to compile all model packs.)")
    ap.add_argument("-f", "--support-feature", type=str, required=False, default="", help="# feature to compile. (Enter a feature to compile.)")
    ap.add_argument("-c", "--support-communication", type=str, required=False, default="", help="# BLE or Wi-Fi to compile. (The default is Wi-Fi.)")
    args = vars(ap.parse_args())

    configIn = open(pioConfig, 'r')
    configStr = configIn.read()
    configIn.close()
    if str(args["support_communication"]) != "":
        logging.info('Setting model type ' + str(args["support_communication"]) + '...')
        configStr, hitCount = re.subn('OSW_FEATURE_WIFI\n','OSW_FEATURE_BLE\n',configStr)
        if hitCount == 0:
            logging.error('Error on setting build flag for BLE!')
            exit(5)
    if str(args["support_feature"]) != "":
        logging.info('Setting flag ' + str(args["support_feature"]) + '...')
        configStr, hitCount = re.subn('build_flags =','build_flags =\n    -D '+str(args["support_feature"]),configStr)
        if hitCount == 0:
            logging.error('Error on setting build feature flag!')
            exit(6)
    configOut = open(pioConfig, 'w')
    configOut.write(configStr)
    configOut.close()

    #if you want all-language packs
    if args["support_language"] == "all" and args["support_model"] == "all":    
        for lang in languages:
            for edition in editions:
                compile_model(lang,edition)
    elif args["support_language"] == "all" and args["support_model"] != "all":
        for lang in languages:
            compile_model(lang, args["support_model"])
    elif args["support_language"] != "all" and args["support_model"] == "all":
        for edition in editions:
            compile_model(args["support_language"], edition)
    else :
        compile_model(args["support_language"], args["support_model"])
