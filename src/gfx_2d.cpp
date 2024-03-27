
#include <Arduino.h>

#include "gfx_2d.h"
#include "gfx_util.h"
#include "math_angles.h"

#include <stdio.h>

void Graphics2D::enableBuffer() {
    drawPixelCallback = NULL;
    uint16_t numChunks = height / chunkHeight;
    buffer = new uint16_t* [numChunks];
    if (isRound) {
        missingPixelColor = rgb565(128, 128, 128);
        chunkXOffsets = new uint16_t[numChunks];
        chunkWidths = new uint16_t[numChunks];
        for (uint16_t i = 0; i < numChunks; i++) {
            uint16_t y = i * chunkHeight;
            float y1 = (y + (y < height / 2 ? chunkHeight : 0)) - height / 2.0f;
            float d = sqrtf(120 * 120 - y1 * y1);

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

void Graphics2D::disableBuffer(DrawPixel* callback) {  //
    delay(1000);
    drawPixelCallback = callback;
    int32_t numChunks = height / chunkHeight;
    for (int32_t i = numChunks - 1; i >= 0; --i) {
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

Graphics2D::~Graphics2D() {
    int32_t numChunks = height / chunkHeight;
    for (int32_t i = 0; i < numChunks; i++) {
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

void Graphics2D::drawPixelClipped(int32_t x, int32_t y, uint16_t color) {
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

uint16_t Graphics2D::getPixel(uint16_t x, uint16_t y) {
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

bool Graphics2D::isInsideChunk(uint16_t x, uint16_t y) {
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
void Graphics2D::drawHLine(int32_t x, int32_t y, uint16_t w, uint16_t color) {
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
void Graphics2D::drawVLine(int32_t x, int32_t y, uint16_t h, uint16_t color) {
    for (uint16_t i = 0; i < h; i++) {
        drawPixel(x, y + i, color);
    }
}

void Graphics2D::drawFrame(int32_t x, int32_t y, uint16_t w, uint16_t h, uint16_t color) {
    drawHLine(x, y, w, color);
    drawHLine(x, y + h, w, color);
    drawVLine(x, y, h, color);
    drawVLine(x + w, y, h, color);
}

void Graphics2D::fillFrame(int32_t x0, int32_t y0, uint16_t w, uint16_t h, uint16_t color) {
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
void Graphics2D::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color) {  // see p3dt_gfx_2d_license.txt
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
        tmp = dx; dx = dy; dy = tmp;
        tmp = x1; x1 = y1; y1 = tmp;
        tmp = x2; x2 = y2; y2 = tmp;
    }
    if (x1 > x2) {
        tmp = x1; x1 = x2; x2 = tmp;
        tmp = y1; y1 = y2; y2 = tmp;
    }
    err = dx / 2;
    if (y2 > y1)
        ystep = 1;
    else
        ystep = -1;
    y = y1;

    if (x2 == 0xffff)
        x2--;

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
     * Draw a thin anti-aliased line from (x1,y1) to (x2,y2) point with color
     *
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @param color
     */
void Graphics2D::drawLineAA(int32_t x0, int32_t y0, int32_t x1, int32_t y1, const uint16_t color) {
    // anti-aliased line

    int dx = abs(x1-x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1-y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx-dy, e2, x2;                               /* error value e_xy */
    int ed = dx+dy == 0 ? 1 : hypotf(dx, dy); //sqrtf(dx*dx+dy*dy);

    for ( ; ; ){                                                 /* pixel loop */
        drawPixelAA(x0, y0, color, 255-255*abs(err-dx+dy)/ed);
        e2 = err; 
        x2 = x0;
        if (2*e2 >= -dx) {                                            /* x step */
            if (x0 == x1) 
                break;
            if (e2+dy < ed) 
                drawPixelAA(x0, y0+sy, color, 255-255*(e2+dy)/ed);
            err -= dy; 
            x0 += sx;
        }
        if (2*e2 <= dy) {                                             /* y step */
            if (y0 == y1) 
                break;
            if (dx-e2 < ed) 
                drawPixelAA(x2+sx, y0, color, 255-255*(dx-e2)/ed);
            err += dx; 
            y0 += sy;
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
void Graphics2D::drawThickLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t radius, uint16_t color,
                    bool highQuality) {  // see p3dt_gfx_2d_license.txt

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
        tmp = dx; dx = dy; dy = tmp;
        tmp = x1; x1 = y1; y1 = tmp;
        tmp = x2; x2 = y2; y2 = tmp;
    }
    if (x1 > x2) {
        tmp = x1; x1 = x2; x2 = tmp;
        tmp = y1; y1 = y2; y2 = tmp;
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

/**
 * @brief Draw an anti-aliased line between (x1,y1) and (x2,y2) with a thicknes of line_width and with a specific color
 *
 * @param x1 x-axis of the start point
 * @param y1 y-axis of the start point
 * @param x2 x-axis of the end point
 * @param y2 y-axis of the end point
 * @param line_width thickness of the line
 * @param color color code use to draw the line.
 */
void Graphics2D::drawThickLineAA(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t line_width, 
                                 const uint16_t color, LINE_END_OPT eol) {  
    // thanks to https://github.com/foo123/Rasterizer   

    int32_t tmp, 
            sx, sy,
            w, w2, wx, wy,
            wsx, wsy,
            dx, dy, n,
            xa, xb, xc, xd,
            ya, yb, yc, yd;

    /*
    given line's thickness w and dx,dy of line, wx, wy are computed as:
    n = hypot(dx, dy)
    w2 = (w-1)/2
    wx = dy*w2/n
    wy = dx*w2/n
    */

    w = line_width;
    w2 = (w-1)/2;

    if (x0 > x1) {
        tmp = x0; x0 = x1; x1 = tmp;
        tmp = y0; y0 = y1; y1 = tmp;
    }

    sx = 1;
    sy = y1 > y0 ? 1 : -1;

    dx = abs(x1 - x0);
    dy = abs(y1 - y0);

    if (dx == 0)
        return fillBoxHV(x0 - w2, y0, x1 - w2 + w, y1, color);
    else if (dy == 0)
        return fillBoxHV(x0, y0 - w2, x1, y1 - w2 + w, color);

    n = hypotf(dx, dy) + 0.5f; //sqrtf(dx*dx + dy*dy) + 0.5f;
    // more readable: wx = (dy*w/2 + (n/2))/n; where (n/2) is for rounding
    wx = (dy*w+n)/2/n; // +1 is for rounding of /2 
    wy = (dx*w+n)/2/n;

//    printf("xxx x0=%d, y0=%d, x1=%d, y1=%d, dx=%d, dy=%d, w2=%d, wx=%d, wy=%d, %d, %d\n", x0, y0, x1, y1, dx, dy, w2, wx, wy);

/*
      wx      .b
    +-----.(0)  .
wy  |.a   | .     .
       .  |   .     .f
          .     .     .
       dy | .g    .     .d
          +---.-----.(1)
            dx  .c

a: y0 + wsy - y0 = -(x - x0)/m => x = x0 - m*wsy: (xs-wsx, y0+wsy)
b: y0 - wsy - y0 = -(x - x0)/m => x = x0 + m*wsy: (xs+wsx, y0-wsy)
c: y1 + wsy - y1 = -(x - x1)/m => x = x1 - m*wsy: (xe-wsx, y1+wsy)
d: y1 - wsy - y1 = -(x - x1)/m => x = x1 + m*wsy: (xe+wsx, y1-wsy)
*/

    wsx = sx*wx;
    wsy = sy*wy;

    xa = x0 - wsx;
    ya = y0 + wsy;
    xb = x0 + wsx;
    yb = y0 - wsy;
    xc = x1 - wsx;
    yc = y1 + wsy;
    xd = x1 + wsx;
    yd = y1 - wsy;

//    printf("xxx xa=%d,ya=%d    xb=%d,yb=%d    xc=%d,yc=%d    xd=%d,yd=%d\n", xa,ya, xb,yb, xc,yc, xd,yd);

    // outline
    drawLineAA(xa, ya, xb, yb, color);
    drawLineAA(xb, yb, xd, yd, color);
    drawLineAA(xd, yd, xc, yc, color);
    drawLineAA(xc, yc, xa, ya, color);

    // fill
    drawFilledTriangle(xb, yb, xa, ya, xc, yc, color);
    drawFilledTriangle(xb, yb, xd, yd, xc, yc, color);

    switch (eol) {
        case STRAIGHT_END:
            break;
        case ROUND_END:  // circle at end
            fillCircleAA(x0, y0, (line_width)/2, color);
            fillCircleAA(x1, y1, (line_width)/2, color);
            break;
        case TRIANGLE_END:
            {
            // still experimental and not finished
            int32_t x, y;
            int32_t xo = sx * line_width * dx / n;
            int32_t yo = sy * line_width * dy / n;
            x = x0 - xo;
            y = y0 - yo;
            drawFilledTriangle(xa, ya, xb, yb, x, y, color/2);
            x = x1 + xo;
            y = y1 + yo;
            drawFilledTriangle(xc, yc, xd, yd, x, y, color/2);
            }
            break;
        default:
            break;
    }
    
/*
    // Center pixel
    drawPixel(x0, y0, rgb565(255,255,0));
    drawPixel(x1, y1, rgb565(255,255,0));

    // Draw vertices
    drawPixel(xa, ya, rgb565(0,0,255));
    drawPixel(xb, yb, rgb565(0,0,255));
    drawPixel(xd, yd, rgb565(0,0,255));
    drawPixel(xc, yc, rgb565(0,0,255));
*/
}

void Graphics2D::drawFilledTriangle(int32_t ax, int32_t ay, int32_t bx, int32_t by, int32_t cx, int32_t cy, const uint16_t color) {
    int32_t tmp, 
            x, xx, y,
            xac, xab, xbc,
            yac, yab, ybc,
            zab, zbc;

    // correction if tirangles edge AC is rising
    int32_t corr = cy-ay < 0 ? 0 : 1;

    if (ay > by) {
        tmp = ax; ax = bx; bx = tmp;
        tmp = ay; ay = by; by = tmp; 
    }
    if (ay > cy) {
        tmp = ax; ax = cx; cx = tmp;
        tmp = ay; ay = cy; cy = tmp; 
    }
    if (by > cy) {
        tmp = bx; bx = cx; cx = tmp;
        tmp = by; by = cy; cy = tmp; 
    }

    yac = cy - ay;
    if (yac == 0)
    {
        // line or single point
        y = ay;
        x = min(ax, min(bx, cx));
        xx = max(ax, max(bx, cx));
        return fillBoxHV(x, y, xx, y, color);
    }

    yab = by - ay;
    ybc = cy - by;
    xac = cx - ax;
    xab = bx - ax;
    xbc = cx - bx;

    zab = yab == 0;
    zbc = ybc == 0;

    for (y = ay; y < cy; ++y)
    {
        if (y < by) { // upper part
            if (zab) {
                x  = ax;
                xx = bx;
            } else {
                x  = (xac*(y - ay))/yac + ax;
                xx = (xab*(y - ay))/yab + ax + corr;
            }
        } else { // lower part
            if (zbc) {
                x  = bx;
                xx = cx;
            } else {
                x  = (xac*(y - ay))/yac + ax;
                xx = (xbc*(y - by))/ybc + bx + corr;
            }
        }
        if (x > xx) { // left handed ;-)
            tmp = x; x = xx; xx = tmp;           
        }

        if (abs(xx-x) <= 0) {
            drawPixel(x, y, color);
        } else {
            for (x = x; x < xx; ++x) 
                drawPixel(x, y, color);
        }
    }
}


/**
  * @brief Draw a horizontal/vertical box (x1,y1) and (x2,y2) with a specific color
  *
  * @param x1 x-axis of the start point
  * @param y1 y-axis of the start point
  * @param x2 x-axis of the end point
  * @param y2 y-axis of the end point
  * @param color color code use to fill the box.
  */
void Graphics2D::fillBoxHV(int32_t x0, int32_t y0, int32_t x1, int32_t y1, const uint16_t color){
    int sx, sy;
    sx = x1 > x0 ? 1 : -1;
    sy = y1 > y0 ? 1 : -1;

    for (int32_t x = x0; x != x1; x += sx)
        for (int32_t y = y0; y != y1; y += sy)
            drawPixel(x, y, color);
}

void Graphics2D::drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

/*
 * "Complex" Stuff:
 */

void Graphics2D::_drawCircleSection(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t color,
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
void Graphics2D::drawCircle(int16_t x0, int16_t y0, int16_t rad, uint16_t color,
                CIRC_OPT option) {  // see p3dt_gfx_2d_license.txt

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

/**
 * @brief Draw an anti-aliased circle with thicknes
 *
 * @param x0 x-axis of the center of the circle
 * @param y0 y-axis of the center of the circle
 * @param r radius of the circle
 * @param bw thickness of th circle
 * @param color color code of the circle
 */
void Graphics2D::drawCircleAA(int16_t off_x, int16_t off_y, int16_t r, int16_t bw, uint16_t color) { 
    int x0 = -r;
    int y0 = -r;
    int x1 = r;
    int y1 = r;

    // for a filled circle
    if (bw >= r || bw <= 0)
        bw = r - 1;

    int o_diam = 2*r; // outer diameter
    const int odd_diam = 0; // o_diam&1; // odd diameter
    int a2 = 2*r-2*bw;
    int dx = 4*(o_diam-1)*o_diam*o_diam;
    int dy = 4*(odd_diam-1)*o_diam*o_diam;  // error increment
    int err = odd_diam*o_diam*o_diam;
    int dx2, dy2, e2, ed;

    if ((bw-1)*(2*o_diam-bw) > o_diam*o_diam) {
        a2 = 0;
        bw = o_diam/2;
    }

    if (x0 > x1) { // if called with swapped points
        x0 = x1;
        x1 += o_diam;
    }       
    if (y0 > y1)
        y0 = y1;     // .. exchange them
    if (a2 <= 0)
        bw = o_diam; // filled ellipse
    e2 = bw;
    bw = x0+bw-e2;
    dx2 = 4*(a2+2*e2-1)*a2*a2;
    dy2 = 4*(odd_diam-1)*a2*a2;
    e2 = dx2*e2;
    y0 += (o_diam+1)>>1;
    y1 = y0-odd_diam+1;                              // starting pixel
    int a1;
    a1 = 8*o_diam*o_diam;
    a2 = 8*a2*a2;

    int i = 0;

    do {
        for (;;) {
            if (err < 0 || x0 > x1) {
                i = x0;
                break;
            }
            i = dx < dy ? dx : dy;
            ed = dx > dy ? dx : dy;

//            ed += 2*ed*i*i/(4*ed*ed+i*i+1)+1;// approx ed=sqrtf(dx*dx+dy*dy)

            int alpha = 255 - 255*err/ed;                             // outside anti-aliasing
            drawPixelAA(off_x + x0, off_y + y0,     color, alpha); // 3. quadrant
            drawPixelAA(off_x + x0, off_y + y1 - 1, color, alpha); // 2. quadrant
            drawPixelAA(off_x + x1, off_y + y0,     color, alpha);  // 1. quadrant
            drawPixelAA(off_x + x1, off_y + y1-1,   color, alpha); /// 4. quadrant

            if (err+dy+a1 < dx) {
                i = x0+1;
                break;
            }
            ++x0;
            --x1;
            err -= dx;
            dx -= a1;  // x error increment
        }
        for ( ; i <= bw && 2*i <= x0+x1; ++i) {  // fill line pixel
            drawPixel(off_x + i,           off_y + y0,     color);  // 3. quadrant
            drawPixel(off_x + i,           off_y + y1 - 1, color);  // 2. quadrant
            drawPixel(off_x + x0 + x1 - i, off_y + y1 - 1, color); // 1. quadrant
            drawPixel(off_x + x0 + x1 - i, off_y + y0,     color);  // 4. quadrant
        }
        while (e2 > 0 && 2*bw <= x0+x1) {               // inside anti-aliasing
            i = dx2 < dy2 ? dx2 : dy2;
            ed = dx2 > dy2 ? dx2 : dy2;

            ed += 2*ed*i*i/(4*ed*ed+i*i);  // approx ed=sqrtf(dx*dx+dy*dy)

            drawPixel(off_x + bw,           off_y + y0,     color); // 3. quadrant
            drawPixel(off_x + bw,           off_y + y1 - 1, color); // 2. quadrant
            drawPixel(off_x + x0 + x1 - bw, off_y + y1 - 1, color); // 1. quadrant
            drawPixel(off_x + x0 + x1 - bw, off_y + y0,     color); // 4. quadrant
            if (e2+dy2+a2 < dx2)
                break;
            ++bw;
            e2 -= dx2;
            dx2 -= a2; // x error increment
        }
        dy2 += a2;
        e2 += dy2;
        dy += a1;    // y step
        err += dy;
        y0++;
        y1--;
    } while (x0 < x1);

    if (y0-y1 <= o_diam)
    {
        if (err > dy+a1) {
            y0--;
            y1++;
            dy -= a1;
            err -= dy;
        }
        for (; y0-y1 <= o_diam; err += dy += a1) { // too early stop of flat ellipses
            int alpha = 255 - 255*err/a1;  // -> finish tip of ellipse
            drawPixelAA(off_x + x0, off_y + y0, color, alpha); // 2. quadrant
            drawPixelAA(off_x + x1, off_y + y0, color, alpha); // 1. quadrant
            drawPixelAA(off_x + x0, off_y + y1, color, alpha); // 3. quadrant
            drawPixelAA(off_x + x1, off_y + y1, color, alpha); // 4. quadrant
            ++y0;
            --y1;
        }
    }
}

void Graphics2D::_fillCircleSection(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t color,
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

void Graphics2D::fillCircle(uint16_t x0, uint16_t y0, uint16_t rad, uint16_t color,
                CIRC_OPT option) {  // see p3dt_gfx_2d_license.txt

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

void Graphics2D::_drawEllipseSection(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t color,
                            CIRC_OPT option) {  // see p3dt_gfx_2d_license.txt

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

void Graphics2D::drawEllipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, uint16_t color,
                    CIRC_OPT option) {  // see p3dt_gfx_2d_license.txt

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

void Graphics2D::_fillEllipseSection(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t color,
                            CIRC_OPT option) {  // see p3dt_gfx_2d_license.txt

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

void Graphics2D::fillEllipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, uint16_t color,
                    CIRC_OPT option) {  // see p3dt_gfx_2d_license.txt

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

void Graphics2D::drawRFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r,
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

void Graphics2D::fillRFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r,
                uint16_t color) {  // see p3dt_gfx_2d_license.txt
    //Prevent infinite looping
    if(h < 2 * r) return;

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
void Graphics2D::drawNTicks(int16_t cx, int16_t cy, int16_t r1, int16_t r2, int16_t nTicks, uint16_t color, int16_t skip_every_nth) {
    const float deltaAngle = 360.0f / nTicks;
    for (int h = nTicks; h >= 0; --h) {
        if (h % skip_every_nth != 0)
            drawTick(cx, cy, r1, r2, h * deltaAngle, color);
    }
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

void Graphics2D::drawNTicksAA(int16_t cx, int16_t cy, int16_t r1, int16_t r2, int16_t nTicks, uint16_t color, int16_t skip_every_nth) {
    const int deltaAngle = 360.0f / nTicks;
    for (int h = nTicks - 1; h >= 0; --h) {
        if (h % skip_every_nth != 0)
            drawTickAA(cx, cy, r1, r2, h * deltaAngle, color);
    }
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
 * @param anti_alias
 */
void Graphics2D::drawArc(int16_t cx, int16_t cy, float start, float stop, int16_t steps, int16_t radius, int16_t lineRadius,
                uint16_t color, bool highQuality, bool anti_alias) {

    int32_t old_x = radius * 4; // impossible value
    int32_t old_y = radius * 4;
    for (float alpha = start; alpha <= stop; alpha += 0.1f) {
        int32_t x = rpx(cx, radius, alpha);
        int32_t y = rpy(cy, radius, alpha);
        if (old_x != x || old_y != y) {
            if (anti_alias)
                fillCircleAA(x, y, lineRadius, color);
            else
                fillCircle(x, y, lineRadius, color);
        }
        old_x = x;
        old_y = y;
    }
    return;
}

void Graphics2D::drawBWBitmap(int16_t x0, int16_t y0, int16_t cnt, int16_t h, uint8_t* bitmap, uint16_t color,
                    uint16_t bgColor, bool drawBackground) {
    // cnt: Number of bytes of the bitmap in horizontal direction. The width of the bitmap is cnt*8.
    // h: Height of the bitmap.

    for (int32_t x = 0; x < cnt; x++) {
        for (int32_t y = 0; y < h; y++) {
            uint8_t bits = bitmap[x + y * cnt];
            for (uint8_t b = 1; b <= 8; b++) {
                if (bits & (1 << (8 - b))) {
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
void Graphics2D::fill(uint16_t color) {
    for (int16_t x = width-1; x >= 0; --x) {
        for (int16_t y = height-1; y >= 0 ; --y) {
            drawPixel(x, y, color);
        }
    }
}

void Graphics2D::dim(uint8_t amount) {
    for (int16_t x = width-1; x >= 0; --x) {
        for (int16_t y = height-1; y >= 0 ; --y) {
            drawPixel(x, y, dimColor(getPixel(x, y), amount));
        }
    }
}

void Graphics2D::drawGraphics2D(int16_t offsetX, int16_t offsetY, Graphics2D* source) {
    for (int16_t y = source->getHeight()-1; y >= 0; --y) {
        for (int16_t x = source->getWidth() - 1; x >= 0; --x) {
            drawPixel(x + offsetX, y + offsetY, source->getPixel(x, y));
        }
    }
}

void Graphics2D::drawGraphics2D(int16_t offsetX, int16_t offsetY, Graphics2D* source, int16_t sourceOffsetX,
                    int16_t sourceOffsetY, int16_t sourceWidth, int16_t sourceHeight) {
    for (int16_t x = sourceWidth - 1; x >= 0; --x) {
        for (int16_t y = sourceHeight - 1;y >= 0; --y) {
            drawPixel(x + offsetX, y + offsetY, source->getPixel(x + sourceOffsetX, y + sourceOffsetY));
        }
    }
}

// draw scaled by 2x
void Graphics2D::drawGraphics2D_2x(int16_t offsetX, int16_t offsetY, Graphics2D* source) {
    for (int32_t x = source->getWidth() * 2 - 1; x >= 0; --x) {
        for (int32_t y = source->getHeight() * 2 -1; y >= 0; --y) {
            drawPixel(x + offsetX, y + offsetY, source->getPixel(x / 2, y / 2));
        }
    }
}

// draw section scaled by 2x
void Graphics2D::drawGraphics2D_2x(int16_t offsetX, int16_t offsetY, Graphics2D* source, int16_t sourceOffsetX,
                        int16_t sourceOffsetY, int16_t sourceWidth, int16_t sourceHeight) {
    for (int16_t x = sourceWidth * 2 - 1; x >= 0; --x) {
        for (int16_t y = sourceHeight * 2 -1; y >= 0; --y) {
            drawPixel(x + offsetX, y + offsetY, source->getPixel(sourceOffsetX + x / 2, sourceOffsetY + y / 2));
        }
    }
}

#ifdef ROTATE_LEGACY
// this rotate function is faster, but it has artifacts
void Graphics2D::drawGraphics2D_rotatedLegacy(uint16_t offsetX, uint16_t offsetY, Graphics2D* source, uint16_t rotationX,
                                    uint16_t rotationY, float angle) {
    float cosA = cosh(angle);
    float sinA = sinh(angle);
    for (uint16_t x = 0; x < source->getWidth(); x++) {
        for (uint16_t y = 0; y < source->getHeight(); y++) {
            int32_t newX = (x - rotationX) * cosA + (y - rotationY) * sinA;
            int32_t newY = (y - rotationY) * cosA - (x - rotationX) * sinA;
            drawPixel(newX + offsetX, newY + offsetY, source->getPixel(x, y));
        }
    }
}
#endif

void Graphics2D::drawGraphics2D_rotated(int16_t offsetX, int16_t offsetY, Graphics2D* source, int16_t rx, int16_t ry,
                            float angle) {
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    // rotateX = (x - rx) * cosf(angle) + (y - ry) * sinf(angle);
    // rotateY = (y - ry) * cosf(angle) - (x - rx) * sinf(angle);

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
    cosA = cosA; // cosA = cos(-angle);
    sinA = -sinA; //sinA = sin(-angle);
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
