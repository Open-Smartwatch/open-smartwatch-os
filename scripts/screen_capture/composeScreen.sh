#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ];
then
    echo "usage: composeScreen.sh <INPUT-IMAGE> <OUTPUT-IMAGE>"
    exit
fi

TEMPFILE=temp.png

convert -composite -gravity center screenshot-overlay.png $1 $TEMPFILE
if [ ! -d ../../screenshots/ ];
then
	mkdir ../../screenshots/
fi
convert -composite -gravity center screenshot-overlay.png $TEMPFILE screenshot-overlay-mask.png ../../screenshots/$2

if [ -f $TEMPFILE ];
then
    rm $TEMPFILE
fi
if [ -f $1 ];
then
	rm $1
fi
