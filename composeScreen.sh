#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ];
then
    echo "usage: fetchScreen.sh <INPUT-IMAGE> <OUTPUT-IMAGE>"
    exit
fi

TEMPFILE=temp.png

convert -composite -gravity center screenshot-overlay.png $1 $TEMPFILE

convert -composite -gravity center screenshot-overlay.png $TEMPFILE screenshot-overlay-mask.png $2

if [ -f $TEMPFILE ];
then
    rm $TEMPFILE
fi
