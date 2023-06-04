#!/usr/bin/env python3
import os
import re
import json
import argparse

from enum import Enum

class GettableInfo(str, Enum):
    LANG = 'default_language'
    MODEL = 'default_model'
    FLAGS = 'all_flags'
    MODELS = 'all_models'
    LANGS = 'all_languages'

def get(info: GettableInfo):
    returnArray = set()
    if info == GettableInfo.LANG:
        langBasePath = 'include/locales/'
        defaultFile = langBasePath + 'en-US.h'
        if not os.path.exists(defaultFile):
            # Well, instead just take the first header-file in the folder
            for file in os.listdir(langBasePath):
                if file.endswith('.h'):
                    defaultFile = langBasePath + file
                    break
        # Strip the path and the extension
        returnArray.add(os.path.splitext(os.path.basename(defaultFile))[0])
    elif  info == GettableInfo.MODEL:
        with open('platformio.ini') as f:
            for line in f:
                if line.startswith('default_envs'):
                    returnArray.add(line.split('=')[1].strip())
                    break
    elif info == GettableInfo.FLAGS:
        with open('docs/firmware/osw_os.md') as f:
            for line in f:
                match = re.match(r'^`(.+)` \| .+ \| (.+)$', line)
                if match is None:
                    continue
                feature = match.group(1).strip()
                featureRequirements = match.group(2).strip()
                required = [feature] + [req.strip('`,').strip() for req in featureRequirements.split(' ')]
                try:
                    required.remove('-') # remove the "no flags" flag
                except ValueError:
                    pass # no "no flags" flag
                for flag in required:
                    if re.match(r'^[\-|A-Z0-9_]+$', flag) is None:
                        # We drop any requirement that is not a proper flag
                        break
                else:
                    returnArray.add(str('|'.join(required)))
    elif info == GettableInfo.MODELS:
        with open('platformio.ini') as f:
            for line in f:
                match = re.match(r'^\[env:(.+)\]$', line)
                if match is None: continue
                env = match.group(1)
                if env.startswith('GPS'):
                    # GPS is NOT build anymore by default
                    continue
                returnArray.add(env)
    elif info == GettableInfo.LANGS:
        langBasePath = 'include/locales'
        for file in os.listdir(langBasePath):
            if file.endswith('.h'):
                returnArray.add(os.path.splitext(file)[0])
    else:
        raise NotImplementedError('Unknown info: ' + info)
    return returnArray

if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument('info', type=GettableInfo)
    args = ap.parse_args()

    print(json.dumps(list(get(args.info))))