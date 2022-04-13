#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ];
then
    echo "usage: fetchScreen.sh <IP> <FILE-BASE-NAME>"
    exit
fi

bash fetchScreen.sh $1 $2.png

bash composeScreen.sh $2.png $2_osw.png
