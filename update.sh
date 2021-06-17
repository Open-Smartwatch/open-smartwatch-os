#!/bin/bash

RED='\033[1;31m'
GREEN='\033[1;32m'
RESET='\033[0m'

git pull || printf "${RED}open-smartwatch-os update failed${RESET}\n"
git submodule update || printf "${RED}submodules update failed${RESET}\n"

