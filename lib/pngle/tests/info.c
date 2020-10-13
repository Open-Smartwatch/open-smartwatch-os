#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

#include "pngle.h"

int main(int argc, char *argv[])
{
	int fd = argc > 1 ? open(argv[1], O_RDONLY) : 0;
	pngle_t *pngle = pngle_new();

	char buf[1024];
	int remain = 0;
	int len;
	while ((len = read(fd, buf + remain, sizeof(buf) - remain)) > 0) {
		int fed = pngle_feed(pngle, buf, remain + len);
		if (fed < 0) {
			fprintf(stderr, "ERROR: %s\n", pngle_error(pngle));
			return -1;
		}

		remain = remain + len - fed;
		if (remain > 0) memmove(buf, buf + fed, remain);
	}

	pngle_ihdr_t *ihdr = pngle_get_ihdr(pngle);

	fprintf(stderr, "%3d x %3d, colorType: %d, depth %2d, mode %c\n"
		, ihdr->width
		, ihdr->height
		, ihdr->color_type
		, ihdr->depth
		, ihdr->interlace ? 'i' : 'n'
	);

	return 0;
}
