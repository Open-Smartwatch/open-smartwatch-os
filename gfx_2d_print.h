#ifndef P3DT_GFX_2D_PRINT_H
#define P3DT_GFX_2D_PRINT_H

// This is a nasty copy paste job of Arduino_GFX.h, sorry, but it works.

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#include "FakePrint.h"
#include "Fakegfxfont.h"
#include "Fakeglcdfont.c"
#include "Fakepgmspace.h"
#else
#include <Arduino.h>

#include "Print.h"
#include "gfxfont.h"
#include "glcdfont.c"
#endif

#include "gfx_2d.h"
#include "gfx_util.h"
#include "math_angles.h"

#ifdef FAKE_ARDUINO
inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c) {
#ifdef __AVR__
  return &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
#else
  // expression in __AVR__ section may generate "dereferencing type-punned pointer will break strict-aliasing rules"
  // warning In fact, on other platforms (such as STM32) there is no need to do this pointer magic as program memory may
  // be read in a usual way So expression may be simplified
  return gfxFont->glyph + c;
#endif  //__AVR__
}

inline uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont) {
#ifdef __AVR__
  return (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);
#else
  // expression in __AVR__ section generates "dereferencing type-punned pointer will break strict-aliasing rules"
  // warning In fact, on other platforms (such as STM32) there is no need to do this pointer magic as program memory may
  // be read in a usual way So expression may be simplified
  return gfxFont->bitmap;
#endif  //__AVR__
}
#endif

class Graphics2DPrint : public Graphics2D, public Print {
 public:
  Graphics2DPrint(uint16_t w_, uint16_t h_, uint8_t chunkHeight_, bool isRound_ = false, bool allocatePsram_ = false)
      : Graphics2D(w_, h_, chunkHeight_, isRound_, allocatePsram_) {
    cursor_x = 0;
    cursor_y = 0;
    _max_x = w_ - 1;  ///< x zero base bound
    _max_y = h_ - 1;  ///< y zero base bound
    textcolor = rgb565(255, 255, 255);
    textbgcolor = rgb565(0, 0, 0);
    textsize_x = textsize_y = 1;
    wrap = true;
    _cp437 = false;
    gfxFont = NULL;
  }

  // Draw a character
  /**************************************************************************/
  /*!
      @brief  Draw a single character
      @param  x       Bottom left corner x coordinate
      @param  y       Bottom left corner y coordinate
      @param  c       The 8-bit font-indexed character (likely ascii)
      @param  color   16-bit 5-6-5 Color to draw chraracter with
      @param  bg      16-bit 5-6-5 Color to fill background with (if same as color, no background)
  */
  /**************************************************************************/
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg) {
    uint16_t block_w;
    uint16_t block_h;

    if (!gfxFont)  // 'Classic' built-in font
    {
      block_w = 6 * textsize_x;
      block_h = 8 * textsize_y;
      if ((x > _max_x) ||             // Clip right
          (y > _max_y) ||             // Clip bottom
          ((x + block_w - 1) < 0) ||  // Clip left
          ((y + block_h - 1) < 0)     // Clip top
      ) {
        return;
      }

      if (!_cp437 && (c >= 176)) {
        c++;  // Handle 'classic' charset behavior
      }

      // Not required: startWrite();
      for (int8_t i = 0; i < 5; i++) {  // Char bitmap = 5 columns
        uint8_t line = pgm_read_byte(&font[c * 5 + i]);
        for (int8_t j = 0; j < 8; j++, line >>= 1) {
          if (line & 1) {
            if (textsize_x == 1 && textsize_y == 1) {
              drawPixel(x + i, y + j, color);
            } else {
              fillFrame(x + i * textsize_x, y + j * textsize_y, textsize_x - text_pixel_margin,
                        textsize_y - text_pixel_margin, color);
            }
          } else if (bg != color) {
            if (textsize_x == 1 && textsize_y == 1) {
              drawPixel(x + i, y + j, bg);
            } else {
              fillFrame(x + i * textsize_x, y + j * textsize_y, textsize_x - text_pixel_margin,
                        textsize_y - text_pixel_margin, bg);
            }
          }
        }
      }
      if (bg != color) {  // If opaque, draw vertical line for last column
        if (textsize_x == 1 && textsize_y == 1) {
          drawVLine(x + 5, y, 8, bg);
        } else {
          fillFrame(x + 5 * textsize_x, y, textsize_x, 8 * textsize_y, bg);
        }
      }
      // Not required: endWrite();
    } else  // Custom font
    {
      // Character is assumed previously filtered by write() to eliminate
      // newlines, returns, non-printable characters, etc.  Calling
      // drawChar() directly with 'bad' characters of font may cause mayhem!

      c -= (uint8_t)pgm_read_byte(&gfxFont->first);
      GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c);
      uint8_t *bitmap = pgm_read_bitmap_ptr(gfxFont);

      uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
      uint8_t w = pgm_read_byte(&glyph->width), h = pgm_read_byte(&glyph->height),
              xAdvance = pgm_read_byte(&glyph->xAdvance), yAdvance = pgm_read_byte(&gfxFont->yAdvance),
              baseline = yAdvance * 2 / 3;  // TODO: baseline is an arbitrary currently, may be define in font file
      int8_t xo = pgm_read_byte(&glyph->xOffset), yo = pgm_read_byte(&glyph->yOffset);
      uint8_t xx, yy, bits = 0, bit = 0;
      int16_t xo16, yo16;

      if (xAdvance < w) {
        xAdvance = w;  // Don't know why it exists
      }

      if (textsize_x > 1 || textsize_y > 1) {
        xo16 = xo;
        yo16 = yo;
      }

      block_w = xAdvance * textsize_x;
      block_h = yAdvance * textsize_y;
      if ((x > _max_x) ||             // Clip right
          (y > _max_y) ||             // Clip bottom
          ((x + block_w - 1) < 0) ||  // Clip left
          ((y + block_h - 1) < 0)     // Clip top
      ) {
        return;
      }

      // NOTE: Different from Adafruit_GFX design, Adruino_GFX also cater background.
      // Since it may introduce many ugly output, it should limited using on mono font only.
      // Not required: startWrite();
      if (bg != color)  // have background color
      {
        fillFrame(x, y - (baseline * textsize_y), block_w, block_h, bg);
      }
      for (yy = 0; yy < h; yy++) {
        for (xx = 0; xx < w; xx++) {
          if (!(bit++ & 7)) {
            bits = pgm_read_byte(&bitmap[bo++]);
          }
          if (bits & 0x80) {
            if (textsize_x == 1 && textsize_y == 1) {
              drawPixel(x + xo + xx, y + yo + yy, color);
            } else {
              fillFrame(x + (xo16 + xx) * textsize_x, y + (yo16 + yy) * textsize_y, textsize_x - text_pixel_margin,
                        textsize_y - text_pixel_margin, color);
            }
          }
          bits <<= 1;
        }
      }
      // Not required: endWrite();
    }  // End classic vs custom font
  }

  size_t write(uint8_t c) {
    if (!gfxFont) {  // 'Classic' built-in font

      if (c == '\n') {                                               // Newline?
        cursor_x = 0;                                                // Reset x to zero,
        cursor_y += textsize_y * 8;                                  // advance y one line
      } else if (c != '\r') {                                        // Ignore carriage returns
        if (wrap && ((cursor_x + (textsize_x * 6) - 1) > _max_x)) {  // Off right?
          cursor_x = 0;                                              // Reset x to zero,
          cursor_y += textsize_y * 8;                                // advance y one line
        }
        drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor);
        cursor_x += textsize_x * 6;  // Advance x one char
      }
    } else {  // Custom font

      if (c == '\n') {
        cursor_x = 0;
        cursor_y += (int16_t)textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
      } else if (c != '\r') {
        uint8_t first = pgm_read_byte(&gfxFont->first);
        if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
          GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
          uint8_t w = pgm_read_byte(&glyph->xAdvance);
          int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset);  // sic
          if (wrap && ((cursor_x + (textsize_x * w) - 1) > _max_x)) {
            cursor_x = 0;
            cursor_y += (int16_t)textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
          }
          drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor);
          cursor_x += textsize_x * w;
        }
      }
    }
    return 1;
  }
  void setFont(const GFXfont *f) {
    if (f) {           // Font struct pointer passed in?
      if (!gfxFont) {  // And no current font struct?
        // Switching from classic to new font behavior.
        // Move cursor pos down 6 pixels so it's on baseline.
        cursor_y += 6;
      }
    } else if (gfxFont) {  // NULL passed.  Current font struct defined?
      // Switching from new to classic font behavior.
      // Move cursor pos up 6 pixels so it's at top-left of char.
      cursor_y -= 6;
    }
    gfxFont = (GFXfont *)f;
  }
  void setTextSize(uint8_t s) { setTextSize(s, s, 0); }
  void setTextSize(uint8_t s_x, uint8_t s_y) { setTextSize(s_x, s_y, 0); }
  void setTextSize(uint8_t s_x, uint8_t s_y, uint8_t pixel_margin) {
    text_pixel_margin = ((pixel_margin < s_x) && (pixel_margin < s_y)) ? pixel_margin : 0;
    textsize_x = (s_x > 0) ? s_x : 1;
    textsize_y = (s_y > 0) ? s_y : 1;
  }
  void setTextColor(uint16_t c, uint16_t bg) {
    textcolor = c;
    textbgcolor = bg;
  }
  void setTextCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
  }
  int16_t getTextCursorX(void) const { return cursor_x; }
  int16_t getTextCursorY(void) const { return cursor_y; };
  void cp437(bool x = true) { _cp437 = x; }
  void setTextWrap(bool w) { wrap = w; }

  void print2Digits(long n) {
    if (n < 10) {
      print("0");
    }
    print(n);
  }

  uint16_t textCharXOffset(uint16_t numChars) {  // works with defaut font only
    return numChars * 6 * textsize_x;
  }
  uint16_t textCharYOffset(uint16_t numRows) {  // works with defaut font only
    return numRows * 8 * textsize_y;
  }

 protected:
  int16_t _max_x;
  int16_t _max_y;
  int16_t cursor_x;
  int16_t cursor_y;
  uint16_t textcolor;
  uint16_t textbgcolor;
  uint8_t textsize_x;
  uint8_t textsize_y;
  uint8_t text_pixel_margin;
  uint8_t _rotation;
  bool wrap;
  bool _cp437;
  GFXfont *gfxFont;
};

#endif
