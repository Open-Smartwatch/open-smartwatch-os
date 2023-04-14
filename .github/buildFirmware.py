#!/usr/bin/env python3
import re
import os
import shutil
import logging
import argparse
import subprocess
logging.basicConfig(level=logging.INFO)

pioConfig = 'platformio.ini'

# And compile!
def compile_model(lang, edition):
    # Configs
    includeConfig = os.path.join('include', 'config.h')
    
    # Modify configs language
    if lang is not None:
        logging.info('Setting language to ' + lang + '...')
        configIn = open(includeConfig, 'r')
        configStr = configIn.read()
        configIn.close()
        configStr, hitCount = re.subn('("locales/).+(\.h")', r'\1' + lang + r'\2', configStr)
        if hitCount != 1:
            logging.error('Error on setting language (failed to replace key in config)!')
            exit(1)
        configOut = open(includeConfig, 'w')
        configOut.write(configStr)
        configOut.close()
    else:
        import getWorkflowMatrix
        lang = getWorkflowMatrix.get(getWorkflowMatrix.GettableInfo.LANG) # get the default language
        assert len(lang) == 1, 'More than one default language found!'
        lang = list(lang)[0] # get the first element of the set
    
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
            logging.error(res.stdout.decode())
            logging.error(res.stderr.decode())
            exit(2)
        # "Export" firmware.bin
        shutil.copy(os.path.join('.pio', 'build', edition, 'firmware.bin'), os.path.join('.', filename))
    if 'debug' in args.support_build.lower():
        doBuild(True)
    if 'release' in args.support_build.lower():
        doBuild(False)

if __name__ == "__main__":
	   
    ap = argparse.ArgumentParser()
    ap.add_argument("-m", "--support-model", type=str, required=True, help="model to compile")
    ap.add_argument("-l", "--support-language", type=str, required=False, help="language to compile")
    ap.add_argument("-f", "--support-feature", type=str, required=False, default="", help="override feature(s) to compile - provide multiple separated by '|'")
    ap.add_argument("-b", "--support-build", type=str, required=False, default="debug,release", help="compile release or debug")
    args = ap.parse_args()

    if len(args.support_feature) > 0:
        overwriteFeatures = set(args.support_feature.split('|'))
        logging.info(f'Setting flags to {overwriteFeatures}...')
        # Setup build flag (using the config file via replacing, as platformio does not allow setting the property using Python)
        configIn = open(pioConfig, 'r')
        configStr = configIn.read()
        configIn.close()
        # Build the feature-string
        featureStr = ''
        for feature in overwriteFeatures:
            featureStr += f'    -D {feature}\n'
        # This will replace all defines from the build_flags line - except the first one (which is the platform header)
        configStr, hitCount = re.subn('build_flags =(\s+-.+)(\s+-.+)*','build_flags =' + r'\1' + '\n' + featureStr, configStr)
        if hitCount == 0:
            logging.error('Error on setting build flag!')
            exit(5)
        configOut = open(pioConfig, 'w')
        configOut.write(configStr)
        configOut.close()

    compile_model(args.support_language, args.support_model)
