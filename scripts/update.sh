#!/bin/bash

echo "This script will help you to update all the submodules and the main repository to the latest master-version."
sleep 2

set -xe

git fetch
git stash # stash any local changes
git switch master
git reset origin/master # using reset instead of pull to avoid merge commits
git checkout -- .
git submodule update --init --recursive
git stash pop # restore any local changes