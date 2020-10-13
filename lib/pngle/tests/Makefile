CFLAGS+=-Os -std=c99
CFLAGS += -I..
CFLAGS += -Wall -Wextra -Wno-misleading-indentation -Wno-unknown-warning-option
# CFLAGS += -DPNGLE_NO_GAMMA_CORRECTION

PNGLE_C=../pngle.c ../miniz.c
PNGLE_H=../pngle.h ../miniz.h

all: png2ppm info

png2ppm: ../examples/png2ppm.c $(PNGLE_C) $(PNGLE_H) Makefile
	$(CC) $(CFLAGS) -o png2ppm ../examples/png2ppm.c $(PNGLE_C) -lm

info: info.c $(PNGLE_C) $(PNGLE_H) Makefile
	$(CC) $(CFLAGS) -o info info.c $(PNGLE_C) -lm

test: png2ppm info
	sh test.sh

clean:
	rm -f png2ppm info
