Pngle
=====

This is a stream based portable **PNG** **L**oader for **E**mbedding, **Pngle**.

## Background

Basically PNG is a memory consuming format from an embedded system perspective, especially on decoding, it mandatory require 32KiB working memory for a sliding window of Deflate, and a scanline buffer (its size depend on image width & format) to reconstruct the image, at least.
Unfortunately, due to the memory requirements, we could say traditional Arduino boards, that uses ATmega328 or ATmega32U4 for example, lack of ability to decode standard PNG images.

Today, we have variety of SoC boards for embedded systems or Arduino that have enough memory to decode PNG images, but we need to be concerned about memory consumption sometimes.
While there are many other PNG decoders, some of them always require the full size of frame-buffer, some of them don't but do require using complicated APIs instead, and some of them are hard to use in Arduino because of deep dependency.
This is why I reinvent the wheel for my own.

## Features & Restrictions

- **All standard types of PNG files are supported**
	- Interlaced files are also supported
	- Tested with PngSuite
- Easy to use
	- **Simple API**
		- Feed PNG binary data with `pngle_feed` API (in the same way as `write` syscall)
	- A super simple single callback interface is used to draw an image
		- You need to render pixel-by-pixel (it's neither line-by-line nor frame-by-frame)
		- Pixel values are always normalized to 8bits/ch x 4ch for convenience (even if they are indexed, grayscaled, ... or 16bits/ch)
		- Drawing x and y values are passed via the interface, so...
			- You can roughly resize an image on-the-fly by adjusting drawing x and y values
			- You can draw an interlaced image as soon as possible (x and y values occur in Adam7 sequence)
- Easy to embed
	- **Reasonably small memory footprint** on runtime
		- Theoretical minimum scanline buffer (depends on width & format) + decompression working memory for Deflate (~43KiB) + α
	- **No frame-buffer required**
		- It simply renders pixel-by-pixel instead, mentioned above
			- If you prefer off-screen canvas, you can allocate the canvas by yourself and draw pixels to it
	- **Less dependency**
		- It only requires `miniz` (don't worry, battery included!)
	- **Portable**
		- Written in C99 with `stdint.h` (but not for `miniz` yet...)
	- MIT License
- **Transparency support**
	- A value of transparency is always available as 4th channel of pixel
		- tRNS chunk is also supported
	- If you need full featured alpha-blending, you can implement it easily (as long as you could know its background source color value, and how to blend them)
- **Gamma correction support** (gAMA chunk only)
	- You can activate the feature by calling `pngle_set_display_gamma` API with display gamma value (Typically 2.2)
	- It require additional memory (depends on image depth, 64KiB at most), math library (libm), and floating point arithmetic to generate gamma lookup table
	- You can remove the feature by defining `PNGLE_NO_GAMMA_CORRECTION` in case of emergency

## Usage & How it works

1. Allocate Pngle object by calling `pngle_new()`
2. Setup draw callback by calling `pngle_set_draw_callback()`
3. Feed PNG binary data by calling `pngle_feed()` until exhausted
4. During the feeding, callback function `on_draw()` (for example) is called repeatedly
5. In the `on_draw()` function, put the pixel on a screen (or wherever you want)
6. Finally, you'll get an image

## Examples

### Generic C
```c
void on_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
    uint8_t r = rgba[0]; // 0 - 255
    uint8_t g = rgba[1]; // 0 - 255
    uint8_t b = rgba[2]; // 0 - 255
    uint8_t a = rgba[3]; // 0: fully transparent, 255: fully opaque

    if (a) printf("put pixel at (%d, %d) with color #%02x%02x%02x\n", x, y, r, g, b);
}

int main(int argc, char *argv[])
{
    pngle_t *pngle = pngle_new();

    pngle_set_draw_callback(pngle, on_draw);

    // Feed data to pngle
    char buf[1024];
    int remain = 0;
    int len;
    while ((len = read(STDIN_FILENO, buf + remain, sizeof(buf) - remain)) > 0) {
        int fed = pngle_feed(pngle, buf, remain + len);
        if (fed < 0) errx(1, "%s", pngle_error(pngle));

        remain = remain + len - fed;
        if (remain > 0) memmove(buf, buf + fed, remain);
    }

    pngle_destroy(pngle);

    return 0;
}
```

### Arduino (for M5Stack)

See [examples/m5stack-png.ino](examples/m5stack-png.ino)

## API

See [pngle.h](pngle.h)


## Author

kikuchan / @kikuchan98


## License

MIT License
