#ifndef P3DT_GFX_2D_H
#define P3DT_GFX_2D_H

#include <Arduino.h>

#include "gfx_util.h"
#include "math_angles.h"

enum CIRC_OPT { DRAW_UPPER_RIGHT, DRAW_UPPER_LEFT, DRAW_LOWER_RIGHT, DRAW_LOWER_LEFT, DRAW_ALL };
enum LINE_END_OPT { STRAIGHT_END, ROUND_END, TRIANGLE_END };

class DrawPixel {
  public:
    DrawPixel() {}
    virtual void drawPixel(int32_t x, int32_t y, uint16_t color) {};
    virtual void drawPixelAA(int32_t x, int32_t y, uint16_t color, uint8_t alpha) {};
};

class Graphics2D {
  public:
    Graphics2D(uint16_t w_, uint16_t h_, uint8_t chunkHeightLd_, bool isRound_ = false, bool allocatePsram_ = false)
        : width(w_), height(h_), chunkHeightLd(chunkHeightLd_), isRound(isRound_), allocatePsram(allocatePsram_) {
        enableBuffer();
        maskEnabled = false;
        maskColor = rgb565(0, 0, 0);
        alphaEnabled = false;
    }

    void enableBuffer();

    inline bool hasBuffer() {
        return drawPixelCallback == NULL;
    }

    void disableBuffer(DrawPixel* callback);

    void fillBuffer(uint16_t color);

    ~Graphics2D();

    inline uint16_t getNumChunks() {
        return numChunks;
    }

    inline uint16_t* getChunk(uint8_t chunkId) {
        return buffer[chunkId];
    }
    inline uint8_t getChunkHeightLd() {
        return chunkHeightLd;
    }
    inline uint16_t getChunkOffset(uint8_t chunkId) {
        return isRound ? chunkXOffsets[chunkId] : 0;
    }
    inline uint16_t getChunkWidth(uint8_t chunkId) {
        return isRound ? chunkWidths[chunkId] : width;
    }
    inline uint16_t getHeight() {
        return height;
    }
    inline uint16_t getWidth() {
        return width;
    }

    inline bool isMaskEnabled() {
        return maskEnabled;
    }
    inline void enableMask(uint16_t color) {
        maskEnabled = true;
        maskColor = color;
    }
    inline void disableMask() {
        maskColor = false;
    }
    inline void enableAlpha(float a) {
        alpha = a;
        alphaEnabled = true;
    }
    inline void disableAlpha() {
        alphaEnabled = false;
    }

    inline void setMissingPixelColor(uint16_t color) {
        missingPixelColor = color;
    }
    inline uint16_t getMissingPixelColor(void) {
        return missingPixelColor;
    }

    // no other functions should be allowed to access the buffer in write mode due to the chunk mapping

    /**
     * @brief Draw a pixel of color 'color' a x-y position.
     *
     * @param x x axis coordinate
     * @param y y axis coordinate
     * @param color color code of the pixel
     */
     void drawPixel(int32_t x, int32_t y, uint16_t color) {
        drawPixelClipped(x, y, color);
    }

    /**
     * @brief Draw a pixel of color 'color' a x-y position and blend it with alpha.
     *
     * @param x x axis coordinate
     * @param y y axis coordinate
     * @param color color code of the pixel
     * @param alpha alpha value to blend with color
     */
    void drawPixelAA(int32_t x, int32_t y, uint16_t color, uint8_t alpha) {
        uint16_t old_color = getPixel(x, y);

        uint16_t new_color = blend(old_color, color, alpha);
        drawPixelClipped(x, y, new_color);
    }

    void drawPixelClipped(int32_t x, int32_t y, uint16_t color);

    uint16_t getPixel(uint16_t x, uint16_t y);

    bool isInsideChunk(uint16_t x, uint16_t y);

    void drawHLine(int32_t x, int32_t y, uint16_t w, uint16_t color);

    void drawVLine(int32_t x, int32_t y, uint16_t h, uint16_t color);

    void drawFrame(int32_t x, int32_t y, uint16_t w, uint16_t h, uint16_t color);

    void fillFrame(int32_t x0, int32_t y0, uint16_t w, uint16_t h, uint16_t color);

    /**
     * Draw line from (x1,y1) to (x2,y2) point with color
     *
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @param color
     */
    void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);

    /**
      * Draw an anti-aliased line from (x1,y1) to (x2,y2) point with color
      *
      * @param x1
      * @param y1
      * @param x2
      * @param y2
      * @param color
      */
    void drawLineAA(int32_t x0, int32_t y0, int32_t x1, int32_t y1, const uint16_t color);

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
    void drawThickLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t radius, uint16_t color, bool highQuality = false);

    /**
     * @brief Draw an anti-aliased line between (x1,y1) and (x2,y2) with a thicknes of line_width and with specific color
     *
     * @param x1 x-axis of the start point
     * @param y1 y-axis of the start point
     * @param x2 x-axis of the end point
     * @param y2 y-axis of the end point
     * @param line_width thickness of the line
     * @param color color code use to draw the line.
     */
    void drawThickLineAA(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t line_width, const uint16_t color, LINE_END_OPT eol = ROUND_END);

    void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
    void drawFilledTriangle(int32_t xa, int32_t ya, int32_t xb, int32_t yb, int32_t xc, int32_t yc, uint16_t color);

    /**
     * @brief Draw a horizontal/vertical box (x1,y1) and (x2,y2) with a specific color
     *
     * @param x1 x-axis of the start point
     * @param y1 y-axis of the start point
     * @param x2 x-axis of the end point
     * @param y2 y-axis of the end point
     * @param color color code use to fill the box.
     */
    void fillBoxHV(int32_t x0, int32_t y0, int32_t x1, int32_t y1, const uint16_t color);

    /*
     * "Complex" Stuff:
     */

    void _drawCircleSection(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t color, CIRC_OPT option);

    /**
     * @brief Draw a circle
     *
     * @param x0 x-axis of the center of the circle
     * @param y0 y-axis of the center of the circle
     * @param rad radius of the circle
     * @param color color code of the circle
     * @param option
     */
    void drawCircle(int16_t x0, int16_t y0, int16_t rad, uint16_t color, CIRC_OPT option = DRAW_ALL); 

    /**
     * @brief Draw an anti-aliased circle with thicknes
     *
     * @param x0 x-axis of the center of the circle
     * @param y0 y-axis of the center of the circle
     * @param r radius of the circle
     * @param bw thickness of th circle
     * @param color color code of the circle
     */
    void drawCircleAA(int16_t off_x, int16_t off_y, int16_t r, int16_t bw, uint16_t color, 
                      int16_t start_angle = 0, int16_t end_angle = 0);

    inline void fillCircleAA(int16_t off_x, int16_t off_y, int16_t r, uint16_t color){ 
        drawCircleAA(off_x, off_y, r, r-1, color);
    }

    void _fillCircleSection(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t color, CIRC_OPT option);
    void fillCircle(uint16_t x0, uint16_t y0, uint16_t rad, uint16_t color, CIRC_OPT option = DRAW_ALL);
    void _drawEllipseSection(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t color, CIRC_OPT option = DRAW_ALL);
    void drawEllipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, uint16_t color, CIRC_OPT option = DRAW_ALL);
    void _fillEllipseSection(uint16_t x, uint16_t y, uint16_t x0, uint16_t y0, uint16_t color, CIRC_OPT option = DRAW_ALL);

    void fillEllipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, uint16_t color, CIRC_OPT option = DRAW_ALL);
    void drawRFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color);
    void fillRFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color);
    
    inline void drawTick(int16_t cx, int16_t cy, int16_t r1, int16_t r2, float angle, uint16_t color) {
        drawLine(rpx(cx, r1, angle), rpy(cy, r1, angle), rpx(cx, r2, angle), rpy(cy, r2, angle), color);
    }

    inline void drawTick(int16_t cx, int16_t cy, int16_t r1, int16_t r2, int angle, uint16_t color) {
        drawLine(rpx(cx, r1, angle), rpy(cy, r1, angle), rpx(cx, r2, angle), rpy(cy, r2, angle), color);
    }

    inline void drawTickAA(int16_t cx, int16_t cy, int16_t r1, int16_t r2, float angle, uint16_t color) {
        drawLineAA(rpx(cx, r1, angle), rpy(cy, r1, angle), rpx(cx, r2, angle), rpy(cy, r2, angle), color);
    }

    inline void drawTickAA(int16_t cx, int16_t cy, int16_t r1, int16_t r2, int32_t angle, uint16_t color) {
        drawLineAA(rpx(cx, r1, angle), rpy(cy, r1, angle), rpx(cx, r2, angle), rpy(cy, r2, angle), color);
    }


    inline void drawThickTick(int16_t cx, int16_t cy, int16_t r1, int16_t r2, float angle, int16_t radius, uint16_t color,
        bool highQuality = false, LINE_END_OPT eol = ROUND_END) {
        if (highQuality)
            drawThickLineAA(rpx(cx, r1, angle), rpy(cy, r1, angle), rpx(cx, r2, angle), rpy(cy, r2, angle), radius, color, eol);
        else
            drawThickLine(rpx(cx, r1, angle), rpy(cy, r1, angle), rpx(cx, r2, angle), rpy(cy, r2, angle), radius, color,
                          highQuality);
    }

    void drawNTicks(int16_t cx, int16_t cy, int16_t r1, int16_t r2, int16_t nTicks, uint16_t color , int16_t skip_every_nth = 361);
    void drawNTicksAA(int16_t cx, int16_t cy, int16_t r1, int16_t r2, int16_t nTicks, uint16_t color, int16_t skip_every_nth = 361);

    /**
     * @brief Draw the ticks around the clock to visualize the hours.
     *
     * 12 ticks will be drawn around the clock.
     *
     * @param cx center x axis
     * @param cy center y axis
     * @param r1 rayon
     * @param r2
     * @param color color code
     */
    inline void drawHourTicks(int16_t cx, int16_t cy, int16_t r1, int16_t r2, uint16_t color, bool anti_alias = true) {
        if (anti_alias)
            drawNTicksAA(cx, cy, r1, r2, 12, color);
        else
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
    inline void drawMinuteTicks(int16_t cx, int16_t cy, int16_t r1, int16_t r2, uint16_t color, bool anti_alias = true) {
        if (anti_alias)
            drawNTicksAA(cx, cy, r1, r2, 60, color, 5);
        else
            drawNTicks(cx, cy, r1, r2, 60, color, 5);
    }

    inline void drawArc(int16_t x, int16_t y, int16_t r1, int16_t r2, float start, float end, uint16_t color, bool anti_alias = true) {
        this->drawArc(x, y, start, end, 1, r1, r2-r1, color, anti_alias); // This _should_ be the equivalent call...
    }

    void drawArc(int16_t cx, int16_t cy, float start, float stop, int16_t steps, int16_t radius, int16_t lineRadius,
                    uint16_t color, bool highQuality = false, bool anti_alias = true);

    void drawBWBitmap(int16_t x0, int16_t y0, int16_t cnt, int16_t h, uint8_t* bitmap, uint16_t color,
                      uint16_t bgColor = 0, bool drawBackground = false);

    void fill(uint16_t color);

    void dim(uint8_t amount);

    void drawGraphics2D(int16_t offsetX, int16_t offsetY, Graphics2D* source);

    void drawGraphics2D(int16_t offsetX, int16_t offsetY, Graphics2D* source, int16_t sourceOffsetX,
                        int16_t sourceOffsetY, int16_t sourceWidth, int16_t sourceHeight);

    // draw scaled by 2x
    void drawGraphics2D_2x(int16_t offsetX, int16_t offsetY, Graphics2D* source);

    // draw section scaled by 2x
    void drawGraphics2D_2x(int16_t offsetX, int16_t offsetY, Graphics2D* source, int16_t sourceOffsetX,
                           int16_t sourceOffsetY, int16_t sourceWidth, int16_t sourceHeight);

#ifdef ROTATE_LEGACY
    // this rotate function is faster, but it has artifacts
    void drawGraphics2D_rotatedLegacy(uint16_t offsetX, uint16_t offsetY, Graphics2D* source, uint16_t rotationX,
                                      uint16_t rotationY, float angle);
#endif

    void drawGraphics2D_rotated(int16_t offsetX, int16_t offsetY, Graphics2D* source, int16_t rx, int16_t ry,
                                float angle);

protected:
    uint16_t** buffer;
    uint16_t numChunks;
    DrawPixel* drawPixelCallback;
    uint16_t* chunkXOffsets;
    uint16_t* chunkWidths;

    /**
     * @brief Width (in pixels) of the display frame
     */
    int32_t width;

    /**
     * @brief Height (in pixels) of the display frame.
     */
    int32_t height;

    uint16_t maskColor;
    uint16_t missingPixelColor;
    bool maskEnabled;
    uint8_t chunkHeightLd; // Height of a chunk is 2^chunkHeightLd
    bool isRound;
    bool alphaEnabled;
    bool allocatePsram;
    float alpha;

    // for caching of isPixelMasked
    void isPixelMaskedByAnglesInit(int32_t off_x, int32_t off_y, int32_t sa, int32_t ea);
    bool isPixelMaskedByAngles(int32_t x, int32_t y);
    int32_t ox, oy;
    int32_t start_angle, end_angle;
    float tan_sa, tan_ea;
};

#endif
