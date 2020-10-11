#ifndef P3DT_GFX_2D_H
#define P3DT_GFX_2D_H

#include "../math/p3dt_math_angles.h"

enum CIRC_OPT { DRAW_UPPER_RIGHT, DRAW_UPPER_LEFT, DRAW_LOWER_RIGHT, DRAW_LOWER_LEFT, DRAW_ALL };

class Graphics2D {
 public:
  Graphics2D(uint16_t w_, uint16_t h_, uint8_t chunk_h_) : width(w_), height(h_), chunk_h(chunk_h_) {
    uint16_t numChunks = height / chunk_h;
    // printf("creating %d chunks", numChunks);
    buffer = new uint16_t*[numChunks];
    for (uint16_t i = 0; i < numChunks; i++) {
      buffer[i] = new uint16_t[width * chunk_h];
    }
  }

  ~Graphics2D() {
    delete[] buffer;  //
  }

  uint16_t numChunks() { return height / chunk_h; }
  uint16_t* getChunk(uint8_t chunk) { return buffer[chunk]; }
  uint8_t getChunkHeight() { return chunk_h; }

  uint16_t getHeight() { return height; }
  uint16_t getWidth() { return width; }

  // no other functions should be allowed to access the buffer in write mode due to the chunk mapping
  void drawPixel(int32_t x, int32_t y, uint16_t color) {
    if (x >= width || y >= height || x < 0 || y < 0) {
      return;
    }
    uint8_t chunkId = y / chunk_h;
    uint16_t chunkY = y - chunkId * chunk_h;
    // printf("chunkid %d, offetY %d for y=%d and chunk_h=%d\n", chunkId, chunkY, y, chunk_h);
    buffer[chunkId][x + chunkY * width] = color;
  }

  uint16_t getPixel(uint16_t x, uint16_t y) {
    if (x >= width || y >= height) {
      return 0;
    }
    uint8_t chunkId = y / chunk_h;
    uint16_t chunkY = y - chunkId * chunk_h;
    // printf("chunkid %d, offetY %d for y=%d and chunk_h=%d\n", chunkId, chunkY, y, chunk_h);
    return buffer[chunkId][x + chunkY * width];
  }

  void drawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color) {
    for (uint16_t i = 0; i < w; i++) {
      drawPixel(x + i, y, color);
    }
  }

  void drawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color) {
    for (uint16_t i = 0; i < h; i++) {
      drawPixel(x, y + i, color);
    }
  }

  void drawFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    drawHLine(x, y, w, color);
    drawHLine(x, y + h, w, color);
    drawVLine(x, y, h, color);
    drawVLine(x + w, y, h, color);
  }

  void fillFrame(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color) {
    for (uint16_t y = y0; y < y0 + h; y++) {
      drawHLine(x0, y, w, color);
    }
  }

  void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {  // see p3dt_gfx_2d_license.txt

    // see p3dt_gfx_2d_license.txt
    uint16_t tmp;
    uint16_t x, y;
    uint16_t dx, dy;
    int16_t err;
    int16_t ystep;

    uint8_t swapxy = 0;

    /* no intersection check at the moment, should be added... */

    if (x1 > x2)
      dx = x1 - x2;
    else
      dx = x2 - x1;
    if (y1 > y2)
      dy = y1 - y2;
    else
      dy = y2 - y1;

    if (dy > dx) {
      swapxy = 1;
      tmp = dx;
      dx = dy;
      dy = tmp;
      tmp = x1;
      x1 = y1;
      y1 = tmp;
      tmp = x2;
      x2 = y2;
      y2 = tmp;
    }
    if (x1 > x2) {
      tmp = x1;
      x1 = x2;
      x2 = tmp;
      tmp = y1;
      y1 = y2;
      y2 = tmp;
    }
    err = dx >> 1;
    if (y2 > y1)
      ystep = 1;
    else
      ystep = -1;
    y = y1;

    if (x2 == 0xffff) x2--;

    for (x = x1; x <= x2; x++) {
      if (swapxy == 0)
        drawPixel(x, y, color);
      else
        drawPixel(y, x, color);
      err -= (uint8_t)dy;
      if (err < 0) {
        y += (uint16_t)ystep;
        err += (uint16_t)dx;
      }
    }
  }

  void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
  }

  /*
   * "Complex" Stuff:
   */

  void _drawCircleSection(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t color,
                          CIRC_OPT option) {  // see p3dt_gfx_2d_license.txt

    if (option == DRAW_UPPER_RIGHT || option == DRAW_ALL) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }

    if (option == DRAW_UPPER_LEFT || option == DRAW_ALL) {
      drawPixel(x0 - x, y0 - y, color);
      drawPixel(x0 - y, y0 - x, color);
    }

    if (option == DRAW_LOWER_RIGHT || option == DRAW_ALL) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }

    if (option == DRAW_LOWER_LEFT || option == DRAW_ALL) {
      drawPixel(x0 - x, y0 + y, color);
      drawPixel(x0 - y, y0 + x, color);
    }
  }

  void drawCircle(uint16_t x0, uint16_t y0, uint16_t rad, uint16_t color,
                  CIRC_OPT option = DRAW_ALL) {  // see p3dt_gfx_2d_license.txt

    float f;
    float ddFx;
    float ddFy;
    float x;
    float y;

    f = 1;
    f -= rad;
    ddFx = 1;
    ddFy = 0;
    ddFy -= rad;
    ddFy *= 2;
    x = 0;
    y = rad;

    _drawCircleSection(x, y, x0, y0, color, option);

    while (x < y) {
      if (f >= 0) {
        y--;
        ddFy += 2;
        f += ddFy;
      }
      x++;
      ddFx += 2;
      f += ddFx;

      _drawCircleSection(x, y, x0, y0, color, option);
    }
  }

  void _fillCircleSection(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t color,
                          CIRC_OPT option) {  // see p3dt_gfx_2d_license.txt

    if (option == DRAW_UPPER_RIGHT || option == DRAW_ALL) {
      drawVLine(x0 + x, y0 - y, y + 1, color);
      drawVLine(x0 + y, y0 - x, x + 1, color);
    }

    if (option == DRAW_UPPER_LEFT || option == DRAW_ALL) {
      drawVLine(x0 - x, y0 - y, y + 1, color);
      drawVLine(x0 - y, y0 - x, x + 1, color);
    }

    if (option == DRAW_LOWER_RIGHT || option == DRAW_ALL) {
      drawVLine(x0 + x, y0, y + 1, color);
      drawVLine(x0 + y, y0, x + 1, color);
    }

    if (option == DRAW_LOWER_LEFT || option == DRAW_ALL) {
      drawVLine(x0 - x, y0, y + 1, color);
      drawVLine(x0 - y, y0, x + 1, color);
    }
  }

  void fillCircle(uint16_t x0, uint16_t y0, uint16_t rad, uint16_t color,
                  CIRC_OPT option = DRAW_ALL) {  // see p3dt_gfx_2d_license.txt

    float f;
    float ddFx;
    float ddFy;
    float x;
    float y;

    f = 1;
    f -= rad;
    ddFx = 1;
    ddFy = 0;
    ddFy -= rad;
    ddFy *= 2;
    x = 0;
    y = rad;

    _fillCircleSection(x, y, x0, y0, color, option);

    while (x < y) {
      if (f >= 0) {
        y--;
        ddFy += 2;
        f += ddFy;
      }
      x++;
      ddFx += 2;
      f += ddFx;

      _fillCircleSection(x, y, x0, y0, color, option);
    }
  }

  void _drawEllipseSection(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t color,
                           CIRC_OPT option = DRAW_ALL) {  // see p3dt_gfx_2d_license.txt

    /* upper right */
    if (option == DRAW_UPPER_RIGHT || option == DRAW_ALL) {
      drawPixel(x0 + x, y0 - y, color);
    }

    /* upper left */
    if (option == DRAW_UPPER_LEFT || option == DRAW_ALL) {
      drawPixel(x0 - x, y0 - y, color);
    }

    /* lower right */
    if (option == DRAW_LOWER_RIGHT || option == DRAW_ALL) {
      drawPixel(x0 + x, y0 + y, color);
    }

    /* lower left */
    if (option == DRAW_LOWER_LEFT || option == DRAW_ALL) {
      drawPixel(x0 - x, y0 + y, color);
    }
  }

  void drawEllipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, uint16_t color,
                   CIRC_OPT option = DRAW_ALL) {  // see p3dt_gfx_2d_license.txt

    float x;
    float y;
    float xchg;
    float ychg;
    float err;
    float rxrx2;
    float ryry2;
    float stopx;
    float stopy;

    rxrx2 = rx;
    rxrx2 *= rx;
    rxrx2 *= 2;

    ryry2 = ry;
    ryry2 *= ry;
    ryry2 *= 2;

    x = rx;
    y = 0;

    xchg = 1;
    xchg -= rx;
    xchg -= rx;
    xchg *= ry;
    xchg *= ry;

    ychg = rx;
    ychg *= rx;

    err = 0;

    stopx = ryry2;
    stopx *= rx;
    stopy = 0;

    while (stopx >= stopy) {
      _drawEllipseSection(x, y, x0, y0, color, option);
      y++;
      stopy += rxrx2;
      err += ychg;
      ychg += rxrx2;
      if (2 * err + xchg > 0) {
        x--;
        stopx -= ryry2;
        err += xchg;
        xchg += ryry2;
      }
    }

    x = 0;
    y = ry;

    xchg = ry;
    xchg *= ry;

    ychg = 1;
    ychg -= ry;
    ychg -= ry;
    ychg *= rx;
    ychg *= rx;

    err = 0;

    stopx = 0;

    stopy = rxrx2;
    stopy *= ry;

    while (stopx <= stopy) {
      _drawEllipseSection(x, y, x0, y0, color, option);
      x++;
      stopx += ryry2;
      err += xchg;
      xchg += ryry2;
      if (2 * err + ychg > 0) {
        y--;
        stopy -= rxrx2;
        err += ychg;
        ychg += rxrx2;
      }
    }
  }

  void _fillEllipseSection(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t color,
                           CIRC_OPT option = DRAW_ALL) {  // see p3dt_gfx_2d_license.txt

    /* upper right */
    if (option == DRAW_UPPER_RIGHT || option == DRAW_ALL) {
      drawVLine(x0 + x, y0 - y, y + 1, color);
    }

    /* upper left */
    if (option == DRAW_UPPER_LEFT || option == DRAW_ALL) {
      drawVLine(x0 - x, y0 - y, y + 1, color);
    }

    /* lower right */
    if (option == DRAW_LOWER_RIGHT || option == DRAW_ALL) {
      drawVLine(x0 + x, y0, y + 1, color);
    }

    /* lower left */
    if (option == DRAW_LOWER_LEFT || option == DRAW_ALL) {
      drawVLine(x0 - x, y0, y + 1, color);
    }
  }

  void fillEllipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, uint16_t color,
                   CIRC_OPT option = DRAW_ALL) {  // see p3dt_gfx_2d_license.txt

    float x;
    float y;
    float xchg;
    float ychg;
    float err;
    float rxrx2;
    float ryry2;
    float stopx;
    float stopy;

    rxrx2 = rx;
    rxrx2 *= rx;
    rxrx2 *= 2;

    ryry2 = ry;
    ryry2 *= ry;
    ryry2 *= 2;

    x = rx;
    y = 0;

    xchg = 1;
    xchg -= rx;
    xchg -= rx;
    xchg *= ry;
    xchg *= ry;

    ychg = rx;
    ychg *= rx;

    err = 0;

    stopx = ryry2;
    stopx *= rx;
    stopy = 0;

    while (stopx >= stopy) {
      _fillEllipseSection(x, y, x0, y0, color, option);
      y++;
      stopy += rxrx2;
      err += ychg;
      ychg += rxrx2;
      if (2 * err + xchg > 0) {
        x--;
        stopx -= ryry2;
        err += xchg;
        xchg += ryry2;
      }
    }

    x = 0;
    y = ry;

    xchg = ry;
    xchg *= ry;

    ychg = 1;
    ychg -= ry;
    ychg -= ry;
    ychg *= rx;
    ychg *= rx;

    err = 0;

    stopx = 0;

    stopy = rxrx2;
    stopy *= ry;

    while (stopx <= stopy) {
      _fillEllipseSection(x, y, x0, y0, color, option);
      x++;
      stopx += ryry2;
      err += xchg;
      xchg += ryry2;
      if (2 * err + ychg > 0) {
        y--;
        stopy -= rxrx2;
        err += ychg;
        ychg += rxrx2;
      }
    }
  }

  void drawRFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r,
                  uint16_t color) {  // see p3dt_gfx_2d_license.txt

    uint16_t xl;
    uint16_t yu;

    xl = x;
    xl += r;
    yu = y;
    yu += r;

    {
      uint16_t yl;
      uint16_t xr;

      xr = x;
      xr += w;
      xr -= r;
      xr -= 1;

      yl = y;
      yl += h;
      yl -= r;
      yl -= 1;

      drawCircle(xl, yu, r, color, DRAW_UPPER_LEFT);
      drawCircle(xr, yu, r, color, DRAW_UPPER_RIGHT);
      drawCircle(xl, yl, r, color, DRAW_LOWER_LEFT);
      drawCircle(xr, yl, r, color, DRAW_LOWER_RIGHT);
    }

    {
      uint16_t ww;
      uint16_t hh;

      ww = w;
      ww -= r;
      ww -= r;
      hh = h;
      hh -= r;
      hh -= r;

      xl++;
      yu++;

      if (ww >= 3) {
        ww -= 2;
        h--;
        drawHLine(xl, y, ww, color);
        drawHLine(xl, y + h, ww, color);
      }

      if (hh >= 3) {
        hh -= 2;
        w--;
        drawVLine(x, yu, hh, color);
        drawVLine(x + w, yu, hh, color);
      }
    }
  }

  void fillRFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r,
                  uint16_t color) {  // see p3dt_gfx_2d_license.txt

    uint16_t xl;
    uint16_t yu;
    uint16_t yl;
    uint16_t xr;

    xl = x;
    xl += r;
    yu = y;
    yu += r;

    xr = x;
    xr += w;
    xr -= r;
    xr -= 1;

    yl = y;
    yl += h;
    yl -= r;
    yl -= 1;

    fillCircle(xl, yu, r, color, DRAW_UPPER_LEFT);
    fillCircle(xr, yu, r, color, DRAW_UPPER_RIGHT);
    fillCircle(xl, yl, r, color, DRAW_LOWER_LEFT);
    fillCircle(xr, yl, r, color, DRAW_LOWER_RIGHT);

    {
      uint16_t ww;
      uint16_t hh;

      ww = w;
      ww -= r;
      ww -= r;
      xl++;
      yu++;

      if (ww >= 3) {
        ww -= 2;
        fillFrame(xl, y, ww, r + 1, color);
        fillFrame(xl, yl, ww, r + 1, color);
      }

      hh = h;
      hh -= r;
      hh -= r;
      // h--;
      if (hh >= 3) {
        hh -= 2;
        fillFrame(x, yu, w, hh, color);
      }
    }
  }

  void drawTick(uint8_t cx, uint8_t cy, uint8_t r1, uint8_t r2, float angle, uint16_t color) {
    drawLine(rpx(cx, r1, angle), rpy(cy, r1, angle), rpx(cx, r2, angle), rpy(cy, r2, angle), color);
  }

  void drawWatchFace(uint8_t cx, uint8_t cy, uint8_t r, uint16_t color) {
    for (uint16_t h = 0; h < 12; h++) {
      drawTick(cx, cy, r * .8, r - 1, h * 30, color);
    }
    for (uint16_t m = 0; m < 60; m++) {
      drawTick(cx, cy, r * .94, r - 1, m * 6, color);
    }
    drawCircle(cx, cy, r, color);
  }

  void drawBWBitmap(uint16_t x0, uint16_t y0, uint16_t cnt, uint16_t h, uint8_t* bitmap, uint16_t color,
                    uint16_t bgColor = 0, bool drawBackground = false) {
    // cnt: Number of bytes of the bitmap in horizontal direction. The width of the bitmap is cnt*8.
    // h: Height of the bitmap.

    for (uint16_t x = 0; x < cnt; x++) {
      for (uint16_t y = 0; y < h; y++) {
        uint8_t bits = bitmap[x + y * cnt];
        for (uint8_t b = 1; b <= 8; b++) {
          if (bits && 1 << (8 - b)) {
            drawPixel(x * 8 + x0 + b, y + y0, color);
          } else if (drawBackground) {
            drawPixel(x * 8 + x0 + b, y + y0, bgColor);
          }
        }
      }
    }
  }

  void fill(uint16_t color) {
    for (uint8_t x = 0; x < width; x++) {
      for (uint8_t y = 0; y < height; y++) {
        drawPixel(x, y, color);
      }
    }
  }

  void scale2xPaint(Graphics2D* source) {
    for (uint8_t x = 0; x < width; x++) {
      for (uint8_t y = 0; y < height; y++) {
        // drawPixel(x, y, source->getPixel(x / 2, y / 2));
      }
    }
  }

 private:
  uint16_t** buffer;
  uint16_t width;
  uint16_t height;
  uint8_t chunk_h;
};

#endif