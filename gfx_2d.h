#ifndef P3DT_GFX_2D_H
#define P3DT_GFX_2D_H

#ifdef FAKE_ARDUINO
#include <iostream>

#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

#include "gfx_util.h"
#include "math_angles.h"

enum CIRC_OPT { DRAW_UPPER_RIGHT, DRAW_UPPER_LEFT, DRAW_LOWER_RIGHT, DRAW_LOWER_LEFT, DRAW_ALL };

class DrawPixel {
 public:
  DrawPixel() {}
  virtual void drawPixel(int32_t x, int32_t y, uint16_t color){};
};

class Graphics2D {
 public:
  Graphics2D(uint16_t w_, uint16_t h_, uint8_t chunkHeight_, bool isRound_ = false, bool allocatePsram_ = false)
      : width(w_), height(h_), chunkHeight(chunkHeight_), isRound(isRound_), allocatePsram(allocatePsram_) {
    enableBuffer();
    maskEnabled = false;
    maskColor = rgb565(0, 0, 0);
    alphaEnabled = false;
  }

  void enableBuffer() {
    drawPixelCallback = NULL;
    uint16_t numChunks = height / chunkHeight;
    buffer = new uint16_t*[numChunks];
    if (isRound) {
      missingPixelColor = rgb565(128, 128, 128);
      chunkXOffsets = new uint16_t[numChunks];
      chunkWidths = new uint16_t[numChunks];
      for (uint16_t i = 0; i < numChunks; i++) {
        uint16_t y = i * chunkHeight;
        float y1 = (y + (y < height / 2 ? chunkHeight : 0)) - height / 2.0;
        float d = sqrt(120 * 120 - y1 * y1);

        uint16_t xOffset = 120 - d;
        uint16_t chunkWidth = ceil(d * 2);

        chunkXOffsets[i] = xOffset;
        chunkWidths[i] = chunkWidth;
        // Serial.print("Chunk: ");
        // Serial.println(i);
        // Serial.print("    Width: ");
        // Serial.println(chunkWidth);
        // Serial.print("    chunkHeight: ");
        // Serial.println(chunkHeight);
        // Serial.print("    Size: ");
        // Serial.println(chunkWidth * chunkHeight);
        // buffer[i] = new uint16_t[chunkWidth * chunkHeight];
        if (allocatePsram) {
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
          buffer[i] = (uint16_t*)ps_malloc(width * chunkHeight * sizeof(uint16_t));
#else
          buffer[i] = new uint16_t[width * chunkHeight]();
#endif
        } else {
          buffer[i] = new uint16_t[width * chunkHeight]();
        }
      }
    } else {
      for (uint16_t i = 0; i < numChunks; i++) {
        // buffer[i] = new uint16_t[width * chunkHeight];
        // (uint8_t*) malloc( BufferSize * sizeof(uint8_t) )
        if (allocatePsram) {
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
          buffer[i] = (uint16_t*)ps_malloc(width * chunkHeight * sizeof(uint16_t));
#else
          buffer[i] = (uint16_t*)malloc(width * chunkHeight * sizeof(uint16_t));
#endif
        } else {
          buffer[i] = (uint16_t*)malloc(width * chunkHeight * sizeof(uint16_t));
        }
      }
    }
  }

  bool hasBuffer() { return drawPixelCallback == NULL; }
  void disableBuffer(DrawPixel* callback) {  //
    delay(1000);
    drawPixelCallback = callback;
    uint16_t numChunks = height / chunkHeight;
    for (uint16_t i = 0; i < numChunks; i++) {
      delete[] buffer[i];
      buffer[i] = NULL;
    }
    delete[] buffer;
    buffer = NULL;

    // delete[] chunkXOffsets;
    // chunkXOffsets = NULL;

    // delete[] chunkWidths;
    // chunkWidths = NULL;
  }

  ~Graphics2D() {
    uint16_t numChunks = height / chunkHeight;
    for (uint16_t i = 0; i < numChunks; i++) {
      delete[] buffer[i];
      buffer[i] = NULL;
    }
    delete[] buffer;
    buffer = NULL;

    // delete[] chunkXOffsets;
    // chunkXOffsets = NULL;

    // delete[] chunkWidths;
    // chunkWidths = NULL;
  }

  uint16_t numChunks() { return height / chunkHeight; }
  uint16_t* getChunk(uint8_t chunkId) { return buffer[chunkId]; }
  uint8_t getChunkHeight() { return chunkHeight; }
  uint16_t getChunkOffset(uint8_t chunkId) { return isRound ? chunkXOffsets[chunkId] : 0; }
  uint16_t getChunkWidth(uint8_t chunkId) { return isRound ? chunkWidths[chunkId] : width; }
  uint16_t getHeight() { return height; }
  uint16_t getWidth() { return width; }

  bool isMaskEnabled() { return maskEnabled; }
  void enableMask(uint16_t color) {
    maskEnabled = true;
    maskColor = color;
  }
  void disableMask() { maskColor = false; }
  void enableAlpha(float a) {
    alpha = a;
    alphaEnabled = true;
  }
  void disableAplha() { alphaEnabled = false; }

  void setMissingPixelColor(uint16_t color) { missingPixelColor = color; }
  uint16_t getMissingPixelColor(void) { return missingPixelColor; }

  // no other functions should be allowed to access the buffer in write mode due to the chunk mapping

  /**
   * @brief Draw a pixel of color 'color' a x-y position.
   *
   * @param x x axis coordinate
   * @param y y axis coordinate
   * @param color color code of the pixel
   */
  void drawPixel(int32_t x, int32_t y, uint16_t color) { drawPixelClipped(x, y, color); }

  void drawPixelClipped(int32_t x, int32_t y, uint16_t color) {
    if (x >= width || y >= height || x < 0 || y < 0) {
      return;
    }
    if (maskEnabled && color == maskColor) {
      return;
    }

    // if we have a pixel callback, there is now buffer
    // draw with the callback and return..
    if (drawPixelCallback != NULL) {
      drawPixelCallback->drawPixel(x, y, color);
      return;
    }

    uint8_t chunkId = y / chunkHeight;
    int16_t chunkY = y - chunkId * chunkHeight;

    //
    if (alphaEnabled) {
      if (isRound && isInsideChunk(x, y)) {
        int16_t chunkX = x - chunkXOffsets[chunkId];
        color = blend(buffer[chunkId][chunkX + chunkY * chunkWidths[chunkId]], color, alpha);
      } else {
        color = blend(buffer[chunkId][x + chunkY * width], color, alpha);
      }
    }

    if (isRound && isInsideChunk(x, y)) {
      int16_t chunkX = x - chunkXOffsets[chunkId];
      buffer[chunkId][chunkX + chunkY * chunkWidths[chunkId]] = color;
    } else if (!isRound) {  // fix for round module
      buffer[chunkId][x + chunkY * width] = color;
    }
  }

  uint16_t getPixel(uint16_t x, uint16_t y) {
    if (x >= width || y >= height) {
      return 0;
    }
    uint8_t chunkId = y / chunkHeight;
    uint16_t chunkY = y - chunkId * chunkHeight;
    // printf("chunkid %d, offetY %d for y=%d and chunkHeight=%d\n", chunkId, chunkY, y, chunkHeight);
    if (isRound) {
      // TODO: check if inside chunk
      if (isInsideChunk(x, y)) {
        uint16_t chunkX = x - chunkXOffsets[chunkId];
        return buffer[chunkId][chunkX + chunkY * chunkWidths[chunkId]];
      } else {
        return missingPixelColor;
      }
    } else {
      return buffer[chunkId][x + chunkY * width];
    }
  }

  bool isInsideChunk(uint16_t x, uint16_t y) {
    uint8_t chunkId = y / chunkHeight;
    // uint16_t chunkY = y - chunkId * chunkHeight;
    uint16_t chunkOffset = chunkXOffsets[chunkId];
    uint16_t chunkWidth = chunkWidths[chunkId];
    bool xFit = chunkOffset < x && x < chunkOffset + chunkWidth;
    // y always fits, because we chunk in rows
    return xFit;
  }

  /**
   * @brief Draw an horizontal line from the point (x,y) to an other horizontal point at h pixels
   *
   * @param x x-axis of the start point
   * @param y y-axis of the start point
   * @param w width of the horizontal line
   * @param color color code of the line
   */
  void drawHLine(int32_t x, int32_t y, uint16_t w, uint16_t color) {
    for (uint16_t i = 0; i < w; i++) {
      drawPixel(x + i, y, color);
    }
  }

  /**
   * @brief Draw a vertical line from the bottom point (x,y) to an other vertical point at h pixels
   *
   * @param x x-axis of the start point
   * @param y y-axis of the start point
   * @param h height of the vertical line
   * @param color color code of the line
   */
  void drawVLine(int32_t x, int32_t y, uint16_t h, uint16_t color) {
    for (uint16_t i = 0; i < h; i++) {
      drawPixel(x, y + i, color);
    }
  }

  void drawFrame(int32_t x, int32_t y, uint16_t w, uint16_t h, uint16_t color) {
    drawHLine(x, y, w, color);
    drawHLine(x, y + h, w, color);
    drawVLine(x, y, h, color);
    drawVLine(x + w, y, h, color);
  }

  void fillFrame(int32_t x0, int32_t y0, uint16_t w, uint16_t h, uint16_t color) {
    for (uint16_t y = y0; y < y0 + h; y++) {
      drawHLine(x0, y, w, color);
    }
  }

  /**
   * Draw line from (x1,y1) to (x2,y2) point with color
   *
   * @param x1
   * @param y1
   * @param x2
   * @param y2
   * @param color
   */
  void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color) {  // see p3dt_gfx_2d_license.txt
    // printf("\ndrawLine(%d, %d, %d, %d)",x1,y1,x2,y2);
    // see p3dt_gfx_2d_license.txt
    int32_t tmp;
    int32_t x, y;
    int32_t dx, dy;
    int32_t err;
    int32_t ystep;

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
      err -= abs(dy);
      if (err < 0) {
        y += ystep;
        err += dx;
      }
    }
  }

  /**
   * @brief Draw a line between (x1,y1) and (x2,y2) with a thick of radius and with specific color
   *
   * Radius is a multiple of 4 pixels.
   *
   * @param x1 x-axis of the start point
   * @param y1 y-axis of the start point
   * @param x2 x-axis of the end point
   * @param y2 y-axis of the end point
   * @param radius radius of the line. Example : radius = 1 give a line of 4 px of diameter, radius 2 -> 8px, etc....
   * @param color color code use to draw the line.
   * @param highQuality
   */
  void drawThickLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t radius, uint16_t color,
                     bool highQuality = false) {  // see p3dt_gfx_2d_license.txt

    // see p3dt_gfx_2d_license.txt
    int32_t tmp;
    int32_t x, y;
    int32_t dx, dy;
    int32_t err;
    int32_t ystep;

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
      if (swapxy == 0) {
        if (highQuality) {
          fillCircle(x, y, radius, color);
        } else {
          drawCircle(x, y, radius, color);
          if (radius > 2) {
            drawCircle(x, y, radius - 1, color);
          }
          if (radius > 3) {
            drawCircle(x, y, radius - 2, color);
          }
        }
      } else {
        if (highQuality) {
          fillCircle(y, x, radius, color);
        } else {
          drawCircle(y, x, radius, color);
          if (radius > 2) {
            drawCircle(y, x, radius - 1, color);
          }
          if (radius > 3) {
            drawCircle(y, x, radius - 2, color);
          }
        }
      }

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

  /**
   * @brief Draw a circle
   *
   * @param x0 x-axis of the center of the circle
   * @param y0 y-axis of the center of the circle
   * @param rad radius of the circle
   * @param color color code of the circle
   * @param option
   */
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
    if(h < 2 * r)
      //Prevent infinite looping
      return;

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

  void drawThickTick(uint8_t cx, uint8_t cy, uint8_t r1, uint8_t r2, float angle, uint8_t radius, uint16_t color,
                     bool highQuality = false) {
    drawThickLine(rpx(cx, r1, angle), rpy(cy, r1, angle), rpx(cx, r2, angle), rpy(cy, r2, angle), radius, color,
                  highQuality);
  }

/**
 * @brief Draw N ticks around the clock to visualize the hours.
 *
 * @param cx center x axis
 * @param cy center y axis
 * @param r1 radius from the begin of the tick.
 * @param r2 radius from the end of the tick.
 * @param nTicks number of ticks to draw
 * @param color color code
 */
  void drawNTicks(uint8_t cx, uint8_t cy, uint8_t r1, uint8_t r2, uint8_t nTicks, uint16_t color) {
    const float deltaAngle = 360.0 / nTicks;
    for (uint16_t h = 0; h < nTicks; h++) {
      drawTick(cx, cy, r1, r2, h * deltaAngle, color);
    }
  }
  
  /**
   * @brief Draw 12 ticks around the clock to visualize the hours.
   *
   * @param cx center x axis
   * @param cy center y axis
   * @param r1 radius from the begin of the tick.
   * @param r2 radius from the end of the tick.
   * @param color color code
   */
  void drawHourTicks(uint8_t cx, uint8_t cy, uint8_t r1, uint8_t r2, uint16_t color) {
    drawNTicks(cx, cy, r1, r2, 12, color);
  }

  /**
   * @brief Draw the ticks around the clock to visualize the minutes.
   *
   * 60 ticks will be drawn around the clock.
   *
   * @param cx center x axis
   * @param cy center y axis
   * @param r1 rayon
   * @param r2
   * @param color color code
   */
  void drawMinuteTicks(uint8_t cx, uint8_t cy, uint8_t r1, uint8_t r2, uint16_t color) {
    drawNTicks(cx, cy, r1, r2, 60, color);
  }

  void drawArc(int16_t x, int16_t y, int16_t r1, int16_t r2, float start, float end, uint16_t color) {
    this->drawArc(x, y, start, end, 1, r1, r2-r1, color); // This _should_ be the equivalent call...
  }

  /**
   * Draw an arc
   *
   * @param cx Arc X center coordinates
   * @param cy Arc Y center coordinates
   * @param start Beginning angle of the arc (in deg). O° is equivalent to 12AM
   * @param stop End angle of the arc (in deg).
   * @param steps Number of lines that will compose the arc.
   * @param radius Radius of the arc from the cx/cy center
   * @param lineRadius Radius of the line. Example : radius = 1 give a line of 4 px of diameter, radius 2 -> 8px,
   * etc....
   * @param color Color code of the arc
   * @param highQuality
   */
  void drawArc(int32_t cx, int32_t cy, float start, float stop, uint16_t steps, uint16_t radius, uint8_t lineRadius,
               uint16_t color, bool highQuality = false) {
    int32_t x1 = rpx(cx, radius, start);
    int32_t y1 = rpy(cy, radius, start);
    // printf("\ndraw from %f,%f in %d steps", start, stop, steps);

    float arcLength = stop - start;

    for (uint16_t i = 1; i <= steps; i++) {
      float segmentLength = i * (arcLength / steps);
      // printf("\n rpx(%d, %d, %f +  %f)", cx, radius, start, segmentLength);

      int32_t x2 = rpx(cx, radius, start + segmentLength);
      int32_t y2 = rpy(cy, radius, start + segmentLength);
      // printf("\n gfx2d.drawLine(%d, %d, %d, %d, color);", x1, y1, x2, y2);
      drawThickLine(x1, y1, x2, y2, lineRadius, color, highQuality);
      x1 = x2;
      y1 = y2;
    }
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

  /**
   * @brief Fill all the display with a color.
   *
   * Used for initialisation of the display with a background color.
   *
   * @param color Color code
   */
  void fill(uint16_t color) {
    for (uint16_t x = 0; x < width; x++) {
      for (uint16_t y = 0; y < height; y++) {
        drawPixel(x, y, color);
      }
    }
  }

  void dim(uint8_t amount) {
    for (uint16_t x = 0; x < width; x++) {
      for (uint16_t y = 0; y < height; y++) {
        drawPixel(x, y, dimColor(getPixel(x, y), amount));
      }
    }
  }

  void drawGraphics2D(int32_t offsetX, int32_t offsetY, Graphics2D* source) {
    for (int32_t y = 0; y < source->getHeight(); y++) {
      for (int32_t x = 0; x < source->getWidth(); x++) {
        drawPixel(x + offsetX, y + offsetY, source->getPixel(x, y));
      }
    }
  }

  void drawGraphics2D(int32_t offsetX, int32_t offsetY, Graphics2D* source, int32_t sourceOffsetX,
                      int32_t sourceOffsetY, uint16_t sourceWidth, uint16_t sourceHeight) {
    for (int32_t x = 0; x < sourceWidth; x++) {
      for (int32_t y = 0; y < sourceHeight; y++) {
        drawPixel(x + offsetX, y + offsetY, source->getPixel(x + sourceOffsetX, y + sourceOffsetY));
      }
    }
  }

  // draw scaled by 2x
  void drawGraphics2D_2x(int32_t offsetX, int32_t offsetY, Graphics2D* source) {
    for (int32_t x = 0; x < source->getWidth() * 2; x++) {
      for (int32_t y = 0; y < source->getHeight() * 2; y++) {
        drawPixel(x + offsetX, y + offsetY, source->getPixel(x / 2, y / 2));
      }
    }
  }

  // draw section scaled by 2x
  void drawGraphics2D_2x(uint16_t offsetX, uint16_t offsetY, Graphics2D* source, uint16_t sourceOffsetX,
                         uint16_t sourceOffsetY, uint16_t sourceWidth, uint16_t sourceHeight) {
    for (uint16_t x = 0; x < sourceWidth * 2; x++) {
      for (uint16_t y = 0; y < sourceHeight * 2; y++) {
        drawPixel(x + offsetX, y + offsetY, source->getPixel(sourceOffsetX + x / 2, sourceOffsetY + y / 2));
      }
    }
  }

#ifdef ROTATE_LEGACY
  // this rotate function is faster, but it has artifacts
  void drawGraphics2D_rotatedLegacy(uint16_t offsetX, uint16_t offsetY, Graphics2D* source, uint16_t rotationX,
                                    uint16_t rotationY, float angle) {
    float cosA = cos(angle);
    float sinA = sin(angle);
    for (uint16_t x = 0; x < source->getWidth(); x++) {
      for (uint16_t y = 0; y < source->getHeight(); y++) {
        int32_t newX = (x - rotationX) * cosA + (y - rotationY) * sinA;
        int32_t newY = (y - rotationY) * cosA - (x - rotationX) * sinA;
        drawPixel(newX + offsetX, newY + offsetY, source->getPixel(x, y));
      }
    }
  }
#endif

  void drawGraphics2D_rotated(uint16_t offsetX, uint16_t offsetY, Graphics2D* source, uint16_t rx, uint16_t ry,
                              float angle) {
    float cosA = cos(angle);
    float sinA = sin(angle);
    // rotateX = (x - rx) * cos(angle) + (y - ry) * sin(angle);
    // rotateY = (y - ry) * cos(angle) - (x - rx) * sin(angle);

    // first calculate the bounding box of the new image
    // // top left
    int32_t tl_x = rotateX(0, 0, rx, ry, cosA, sinA);
    int32_t tl_y = rotateY(0, 0, rx, ry, cosA, sinA);
    // // top right
    int32_t tr_x = rotateX(source->getWidth() - 1, 0, rx, ry, cosA, sinA);
    int32_t tr_y = rotateY(source->getWidth() - 1, 0, rx, ry, cosA, sinA);

    // // bottom left
    int32_t bl_x = rotateX(0, source->getHeight() - 1, rx, ry, cosA, sinA);
    int32_t bl_y = rotateY(0, source->getHeight() - 1, rx, ry, cosA, sinA);

    // // bottom right
    int32_t br_x = rotateX(source->getWidth(), source->getHeight(), rx, ry, cosA, sinA);
    int32_t br_y = rotateY(source->getWidth(), source->getHeight(), rx, ry, cosA, sinA);

    // debug: draw rotated image
    // this->drawLine(offsetX + tl_x, offsetY + tl_y, offsetX + tr_x, offsetY + tr_y, rgb565(255, 0, 0));
    // this->drawLine(offsetX + tr_x, offsetY + tr_y, offsetX + br_x, offsetY + br_y, rgb565(255, 0, 0));
    // this->drawLine(offsetX + bl_x, offsetY + bl_y, offsetX + br_x, offsetY + br_y, rgb565(255, 0, 0));
    // this->drawLine(offsetX + bl_x, offsetY + bl_y, offsetX + tl_x, offsetY + tl_y, rgb565(255, 0, 0));

    // determine bounding box
    int32_t boxX = min(tl_x, min(tr_x, min(bl_x, br_x)));
    int32_t boxY = min(tl_y, min(tr_y, min(bl_y, br_y)));
    int32_t boxW = max(tl_x, max(tr_x, max(bl_x, br_x))) - boxX;
    int32_t boxH = max(tl_y, max(tr_y, max(bl_y, br_y))) - boxY;

    // debug: draw bounding box
    // this->drawFrame(boxX + offsetX, boxY + offsetY, boxW, boxH, rgb565(0, 255, 0));
    cosA = cos(-angle);
    sinA = sin(-angle);
    for (int16_t x = boxX; x < boxX + boxW; x++) {
      for (int16_t y = boxY; y < boxY + boxH; y++) {
        if (pointInsideTriangle(x, y, tl_x, tl_y, tr_x, tr_y, br_x, br_y)) {
          int16_t origX = rotateX(x, y, 0, 0, cosA, sinA);
          int16_t origY = rotateY(x, y, 0, 0, cosA, sinA);
          drawPixel(x + offsetX, y + offsetY, source->getPixel(origX + rx, origY + ry));
        } else if (pointInsideTriangle(x, y, tl_x, tl_y, bl_x, bl_y, br_x, br_y)) {
          int16_t origX = rotateX(x, y, 0, 0, cosA, sinA);
          int16_t origY = rotateY(x, y, 0, 0, cosA, sinA);
          drawPixel(x + offsetX, y + offsetY, source->getPixel(origX + rx, origY + ry));
        }
      }
    }
  }

 protected:
  uint16_t** buffer;
  DrawPixel* drawPixelCallback;
  uint16_t* chunkXOffsets;
  uint16_t* chunkWidths;

  /**
   * @brief Width (in pixels) of the display frame
   */
  uint16_t width;

  /**
   * @brief Height (in pixels) of the display frame.
   */
  uint16_t height;

  uint16_t maskColor;
  uint16_t missingPixelColor;
  bool maskEnabled;
  uint8_t chunkHeight;
  bool isRound;
  bool alphaEnabled;
  bool allocatePsram;
  float alpha;
};

#endif
