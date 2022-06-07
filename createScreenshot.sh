#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ];
then
    echo "usage: createScreenshot.sh <IP> <FILE-BASE-NAME>"
    exit
fi

bash screenshotScript/fetchScreen.sh $1 $2.png

bash screenshotScript/composeScreen.sh $2.png $2_osw.png
