#ifndef P3DT_GFX_2D_PRINT_H
#define P3DT_GFX_2D_PRINT_H

// This is (still) a nasty copy paste job of Arduino_GFX.h, sorry, but it works.

#ifdef FAKE_ARDUINO
#include <iostream>

#include "FakeArduino.h"
#include "FakePrint.h"
#include "Fakegfxfont.h"
#include "Fakeglcdfont.h"
#include "Fakepgmspace.h"
#else
#include <Arduino.h>
#include <Arduino_TFT.h>

#include "Print.h"
#include "gfxfont.h"
#include "font/glcdfont.h"
#endif

#include "fonts/ows_font_CEI_8859-15.cpp"
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
    // TODO: fix text wrapping on single character prints
    // wrap = true;
    _cp437 = false;
    gfxFont = nullptr;
    text_x_alignment = _text_alignment::LEFT;
    text_y_alignment = _text_alignment::LEFT;
    text_pixel_margin = 0;  // unused
    _rotation = 0;          // unused
  }

  // helper functions
  size_t getTextLength(const uint8_t *buffer, size_t size) const {
    size_t string_size = 0;
    while (size--) {
      string_size += getCharWidth(*buffer++);
    }
    return string_size;
  }

  size_t getCharWidth(const uint8_t tempC) const {
    if (tempC == '\n') {
      return 0;
    }

    if (gfxFont == nullptr) {
      return 6;  // basic font width.
    } else {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if ((tempC >= first) && (tempC <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, tempC - first);
        return pgm_read_byte(&glyph->xAdvance);
      }
    }
    return 0;
  }

  size_t getCharHeight(const uint8_t tempC) const {
    if (tempC == '\n') {
      return 0;
    }

    if (gfxFont == nullptr) {
      return 8;  // basic font height.
    } else {
      return pgm_read_byte(&gfxFont->yAdvance);
    }
    return 0;
  }

  /*!
      @brief  Draw a single character
      @param  x       Bottom left corner x coordinate
      @param  y       Bottom left corner y coordinate
      @param  c       The 8-bit font-indexed character (likely ascii)
      @param  color   16-bit 5-6-5 Color to draw character with
      @param  bg      16-bit 5-6-5 Color to fill background with (if same as color, no background)
  */
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
        uint8_t line = pgm_read_byte(&OSWfont[c * 5 + i]);
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
      int16_t xo16 = 0, yo16 = 0;

      if (xAdvance < w) {
        xAdvance = w;  // Don't know why it exists
      }

      if (textsize_x > 1 || textsize_y > 1) {
        xo16 = (unsigned char)xo;
        yo16 = (unsigned char)yo;
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

  // manage writing to buffer and virtual from print header
  size_t write(uint8_t c) override {
    // newline can only happen with direct function calls

    if (!gfxFont) {                  // 'Classic' built-in font
      if (c == '\n') {               // Newline?
        cursor_x = 0;                // Reset x to zero,
        cursor_y += textsize_y * 8;  // advance y one line
      } else if (c != '\r') {        // Ignore carriage returns
        drawChar(cursor_x, cursor_y - 7 * (textsize_y), c, textcolor, textbgcolor);
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
          drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor);
          cursor_x += textsize_x * pgm_read_byte(&glyph->xAdvance);
        }
      }
    }
    return 1;
  }

  size_t write(const uint8_t *buffer, size_t size) override {
    // check if it fits && check if there is a /n in the text.
    int16_t temp_cursor_x = cursor_x;
    // int16_t space = 0;
    // TODO: fix text wrapping on single character prints
    // if (wrap) {
    //   if (text_x_alignment == _text_alignment::LEFT) {
    //     space = width - temp_cursor_x;
    //   } else if (text_x_alignment == _text_alignment::RIGHT) {
    //     space = temp_cursor_x;
    //   } else if (text_x_alignment == _text_alignment::CENTER) {
    //     if (temp_cursor_x * 2 > width) {
    //       space = (width - temp_cursor_x) * 2;
    //     } else {
    //       space = temp_cursor_x * 2;
    //     }
    //   }
    // }

    size_t count = 0;         // to keep track of the buffer
    uint16_t msg_length = 0;  // keep track of the message length for wrap
    for (size_t i = 0; i < size; i++) {
      if (buffer[i] == '\n') {
        if ((i - count) > 0) {
          write_nocheck(&buffer[count], (i - count), false);
        }
        write('\n');
        cursor_x = temp_cursor_x;
        count = i + 1;
        msg_length = 0;
      }
      msg_length += getCharWidth(buffer[i]);

      // TODO: fix text wrapping on single character prints
      // if (wrap && msg_length > space) {  // wrap
      //   write_nocheck(&buffer[count], (i - count), false);
      //   write('\n');
      //   cursor_x = temp_cursor_x;
      //   count = i;
      //   i--;  // decrease it one to include the "to large" character
      //   msg_length = 0;
      // }
    }

    if (size == 1) {  // needed for single characters
      write_nocheck(&buffer[count], ((size)-count), true);
    } else if (count != size - 1) {  // needed for center when user doesn't provide an "\n"
      write_nocheck(&buffer[count], ((size)-count), true);
    }

    return count;
  }
  size_t write_nocheck(const uint8_t *buffer, size_t size, bool final) {
    //--> remember the cursor position before printing
    int16_t temp_cursor_x = cursor_x;
    int16_t temp_cursor_y = cursor_y;

    //---> check x alignment
    if (text_x_alignment == _text_alignment::RIGHT) {
      const size_t msg_length = getTextLength(buffer, size);
      cursor_x -= msg_length * textsize_x;
      temp_cursor_x = cursor_x;
    } else if (text_x_alignment == _text_alignment::CENTER) {
      const size_t msg_length = getTextLength(buffer, size);
      cursor_x -= msg_length * textsize_x / 2;
    }

    //-->check y alignment
    if (text_y_alignment == _text_alignment::CENTER) {
      if (!gfxFont)
        cursor_y += (int16_t)textsize_y * 8 / 2;
      else {
        const unsigned char c = '1' - (uint8_t)pgm_read_byte(&gfxFont->first);
        GFXglyph *glyph_temp = pgm_read_glyph_ptr(gfxFont, c);
        cursor_y += (int16_t)textsize_y * (glyph_temp->height / 2);
      }
    } else if (text_y_alignment == _text_alignment::RIGHT) {  // in other words: under baseline
      if (!gfxFont)
        cursor_y += (int16_t)textsize_y * 8;
      else {
        const unsigned char c = '1' - (uint8_t)pgm_read_byte(&gfxFont->first);
        GFXglyph *glyphtemp = pgm_read_glyph_ptr(gfxFont, c);
        cursor_y += (int16_t)textsize_y * (glyphtemp->height);
      }
    }

    // then actually write the buffer.
    size_t n = 0;
    while (size--) {
      n += write(*buffer++);
    }

    //--> then fix the cursor x alignment
    if (text_x_alignment == _text_alignment::RIGHT) {
      cursor_x = temp_cursor_x;
    } else if (text_x_alignment == _text_alignment::CENTER) {
      if (final && buffer[size - 1] != '\n') {
        if (!gfxFont) {
          if (cursor_y == temp_cursor_y) cursor_y = temp_cursor_y + textsize_y * 8;  // advance y one line
        } else {
          if (cursor_y == temp_cursor_y)
            cursor_y = temp_cursor_y + textsize_y * gfxFont->yAdvance;  // advance y one line
        }
        temp_cursor_y = cursor_y;
      }
      cursor_x = temp_cursor_x;
    }

    //--> then fix the cursor y alignment
    if (text_y_alignment != _text_alignment::LEFT) {
      cursor_y = temp_cursor_y;
    }

    return n;
  }

  /**
   * Display a number with a minimum number of digits.
   *
   * Example : yourNumer = 3 and numDigits = 4 display 0003
   *
   * @param yourNumber number to display
   * @param numDigits minimum number of digit to display
   */
  void printDecimal(long yourNumber, int numDigits) {
    for (int i = 1; i < numDigits; i++) {
      if (yourNumber < pow(10, i)) {
        print("0");
      }
    }
    print(yourNumber);
  }

  // set alignment options
  void setTextCenterAligned() { text_x_alignment = _text_alignment::CENTER; }
  void setTextLeftAligned() { text_x_alignment = _text_alignment::LEFT; }
  void setTextRightAligned() { text_x_alignment = _text_alignment::RIGHT; }
  void setTextMiddleAligned() { text_y_alignment = _text_alignment::CENTER; }
  void setTextBottomAligned() { text_y_alignment = _text_alignment::LEFT; }
  void setTextTopAligned() { text_y_alignment = _text_alignment::RIGHT; }

  // set font options
  void clearFont() { setFont(nullptr); }

  /**
   * @brief Can defined a specific font to use.
   * 
   * WARNING : you have to put a resetFont() when you want to use an other font.
   * If you not this font will be used for all the caracters displayed
   * 
   * @param f 
   */
  void setFont(const GFXfont *f) { gfxFont = (GFXfont *)f; }

  /**
   * @brief Set the text size.
   * 
   * Size "1" mean a size of 8px height and 6 px width.
   * Size "2" mean a double size => 18px height x 12px width
   * 
   * @param s Size of the text 
   */
  void setTextSize(uint8_t s) { setTextSize(s, s, 0); }

/**
 * @brief Set the x and y factor size to apply
 * 
 * Example : setTextSize(3,2) will set the text size to a block of 3*8=24px height and 2x6px=12px width
 * 
 * @param s_x x/width size factor
 * @param s_y y/height size factor
 */
  void setTextSize(uint8_t s_x, uint8_t s_y) { setTextSize(s_x, s_y, 0); }

  void setTextSize(uint8_t s_x, uint8_t s_y, uint8_t pixel_margin) {
    text_pixel_margin = ((pixel_margin < s_x) && (pixel_margin < s_y)) ? pixel_margin : 0;
    textsize_x = (s_x > 0) ? s_x : 1;
    textsize_y = (s_y > 0) ? s_y : 1;
  }

  void setTextColor(uint16_t c) { textcolor = textbgcolor = c; }
  void setTextColor(uint16_t c, uint16_t bg) {
    textcolor = c;
    textbgcolor = bg;
  }
  // TODO: fix text wrapping on single character prints
  // void setTextWrap(bool w) { wrap = w; }

  // cursor functions
  /**
   * @brief Set the text cursor position.
   * 
   * The cursor define le left-bottom position of the caracter.
   * 
   * @param x x-axis of the cursor.
   * @param y y-axis of the cursor.
   */
  void setTextCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
  }
  int16_t getTextCursorX() const { return cursor_x; }
  int16_t getTextCursorY() const { return cursor_y; };
  void cp437(bool x = true) { _cp437 = x; }

  uint16_t getTextOfsetColumns(const float numChars) const {  // works with default font only
    return numChars * getCharWidth(' ') * textsize_x;
  }
  uint16_t getTextOfsetRows(const float numRows) const {  // works with default font only
    return numRows * getCharHeight(' ') * textsize_y;
  }

  void resetText() {
    setTextSize(1);
    clearFont();
    setTextLeftAligned();
    setTextBottomAligned();
  }

 protected:
  int16_t _max_x;
  int16_t _max_y;

  /**
   * Distance to the left of the screen (in pixels) of the cursor.
   **/
  int16_t cursor_x;
  /**
   * Distance to the top of the screen (in pixels) of the cursor.
   **/
  int16_t cursor_y;
  uint16_t textcolor;
  uint16_t textbgcolor;
  uint8_t textsize_x;
  uint8_t textsize_y;
  uint8_t text_pixel_margin;
  uint8_t _rotation;
  enum _text_alignment { RIGHT = 0, LEFT, CENTER };
  _text_alignment text_x_alignment;
  _text_alignment text_y_alignment;
  // TODO: fix text wrapping on single character prints
  // bool wrap;
  bool _cp437;

 public:
  GFXfont *gfxFont;
};

#endif
