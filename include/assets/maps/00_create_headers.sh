#!/bin/bash

# for tile in $(ls osm_*.png);
# do
#     echo "const $(xxd -i osm_0_0_0.png)" | sed 's/\]/] PROGMEM/g' > $tile.h
#     echo $(echo $tile | sed 's/\./_/g')
# done

PROGMEM_TILES_H=progmem_tiles.h

function includeHeader () {
    echo "#include \"osm_${1}_${2}_${3}_png.h\""  >> $PROGMEM_TILES_H
    echo "const $(xxd -i osm_${1}_${2}_${3}.png)" | sed 's/\]/] PROGMEM/g' > osm_${1}_${2}_${3}_png.h
}

function setTile () {
    echo "    if (z == $1 && x == $2 && y == $3) { *len = osm_${1}_${2}_${3}_png_len; return osm_${1}_${2}_${3}_png;}"  >> $PROGMEM_TILES_H
}

echo "#ifndef PROGMEM_TILES_H" > $PROGMEM_TILES_H
echo "#define PROGMEM_TILES_H" >> $PROGMEM_TILES_H

includeHeader 0 0 0

includeHeader 1 0 0
includeHeader 1 0 1
includeHeader 1 1 0
includeHeader 1 1 1

includeHeader 2 0 0
includeHeader 2 0 1
includeHeader 2 0 2
includeHeader 2 0 3
includeHeader 2 1 0
includeHeader 2 1 1
includeHeader 2 1 2
includeHeader 2 1 3
includeHeader 2 2 0
includeHeader 2 2 1
includeHeader 2 2 2
includeHeader 2 2 3
includeHeader 2 3 0
includeHeader 2 3 1
includeHeader 2 3 2
includeHeader 2 3 3

echo "const unsigned char* getProgmemTilePng(unsigned int z, unsigned int x, unsigned int y, unsigned int* len) {" >> $PROGMEM_TILES_H
setTile 0 0 0

setTile 1 0 0
setTile 1 0 1
setTile 1 1 0
setTile 1 1 1

setTile 2 0 0
setTile 2 0 1
setTile 2 0 2
setTile 2 0 3
setTile 2 1 0
setTile 2 1 1
setTile 2 1 2
setTile 2 1 3
setTile 2 2 0
setTile 2 2 1
setTile 2 2 2
setTile 2 2 3
setTile 2 3 0
setTile 2 3 1
setTile 2 3 2
setTile 2 3 3
echo "}" >> $PROGMEM_TILES_H

echo "#endif" >> $PROGMEM_TILES_H
