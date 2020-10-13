#!/bin/sh

exec 2>/dev/null

for i in pngsuite/*.png; do
	echo -n "${i}: "
	./info "$i" 2>&1 | tr '\n' ' '

	a=`./png2ppm -g 1 "$i" | pamdepth 255| cksum`
	b=`pngtopnm -gamma 1 -quiet "$i" | ppmtoppm | pamdepth 255 | cksum`

	if [ "x$a" = "x$b" ]; then
		echo "... [32mPASS[m"
	else
		echo "... [31mFAILED[m"
	fi
done
