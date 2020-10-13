#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <math.h>

#include "pngle.h"

uint8_t *img;
uint8_t *counts; // draw counter on (x, y)
int width, height;

double scale_factor = 1.0;

#define UNUSED(x) (void)(x)

void put_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
	uint8_t *p = img + (y * width + x) * 3;
	uint8_t c = ++counts[y * width + x];

	p[0] += (r - p[0]) / c;
	p[1] += (g - p[1]) / c;
	p[2] += (b - p[2]) / c;

}

void rectangle(double x, double y, double w, double h, uint8_t r, uint8_t g, uint8_t b)
{
	int iw = (int)ceil(w);
	int ih = (int)ceil(h);

	int ix, iy;

	for (iy = 0; iy < ih; iy++) {
		for (ix = 0; ix < iw; ix++) {
			put_pixel(x + ix, y + iy, r, g, b);
		}
	}
}

void draw_pixel(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
	UNUSED(pngle);
	UNUSED(w);
	UNUSED(h);
	uint8_t r = rgba[0];
	uint8_t g = rgba[1];
	uint8_t b = rgba[2];
	// uint8_t a = rgba[3]; // ignore

	rectangle(x * scale_factor, y * scale_factor, scale_factor, scale_factor, r, g, b);
}

void init_screen(pngle_t *pngle, uint32_t w, uint32_t h)
{
	UNUSED(pngle);
	width = w * scale_factor;
	height = h * scale_factor;

	img = calloc(width * height, 3);
	counts = calloc(width * height, 1);
}

void flush_screen(pngle_t *pngle) {
	UNUSED(pngle);
	printf("P6\n");
	printf("%u %u\n", width, height);
	printf("255\n");

	fwrite(img, width * height, 3, stdout);
}

int main(int argc, char *argv[])
{
	FILE *fp = stdin;
	extern int optind;
	extern char *optarg;
	int ch;
	double display_gamma = 0;

	while ((ch = getopt(argc, argv, "g:s:h")) != -1) {
		switch (ch) {
		case 'g':
			display_gamma = atof(optarg);
			break;

		case 's':
			scale_factor = atof(optarg);
			break;

		case 'h':
		case '?':
		default:
			fprintf(stderr, "Usage: %s [options] [input.png]\n", argv[0]);
			return 1;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc > 0) {
		fp = fopen(argv[0], "rb");
		if (fp == NULL) {
			fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
			return -1;
		}
	}

	char buf[1024];
	size_t remain = 0;
	int len;

	pngle_t *pngle = pngle_new();

	pngle_set_init_callback(pngle, init_screen);
	pngle_set_draw_callback(pngle, draw_pixel);
	pngle_set_done_callback(pngle, flush_screen);

	pngle_set_display_gamma(pngle, display_gamma);

	while (!feof(fp)) {
		if (remain >= sizeof(buf)) {
			if (argc > 1) fprintf(stderr, "%s: ", argv[1]);
			fprintf(stderr, "Buffer exceeded\n");
			return -1;
		}

		len = fread(buf + remain, 1, sizeof(buf) - remain, fp);
		if (len <= 0) {
			//printf("EOF\n");
			break;
		}

		int fed = pngle_feed(pngle, buf, remain + len);
		if (fed < 0) {
			if (argc > 1) fprintf(stderr, "%s: ", argv[1]);
			fprintf(stderr, "ERROR; %s\n", pngle_error(pngle));
			return -1;
		}

		remain = remain + len - fed;
		if (remain > 0) memmove(buf, buf + fed, remain);
	}

	if (fp != stdin) fclose(fp);

	return 0;
}
