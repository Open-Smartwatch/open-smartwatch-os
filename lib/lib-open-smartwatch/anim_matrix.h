#ifndef ANIM_MATRIX_H
#define ANIM_MATRIX_H
#include "gfx_2d_print.h"

#define MATRIX_STRING_MAX_LENGTH 16

class MatrixString {
 public:
  MatrixString(Graphics2DPrint* gfx, const char* matrixChars = "GATC", uint8_t numMatrixChars = 4,
               uint8_t maxScale = 3) {
    this->gfx = gfx;
    this->matrixChars = matrixChars;
    this->numMatrixChars = numMatrixChars;
    this->maxScale = maxScale;
    randomize();
  }
  void randomize(void) {
    length = random(MATRIX_STRING_MAX_LENGTH - 1) + 1;
    x = 16 + random(gfx->getWidth() - 32);
    scale = random(maxScale) + 1;
    gfx->setTextSize(scale);
    y = 0 - (gfx->getTextOfsetRows(length)) - random(100);
    speed = random(scale) + scale;
    for (uint8_t i = 0; i < length; i++) {
      s[i] = matrixChars[random(numMatrixChars)];
    }
  }

  uint8_t length;
  char s[MATRIX_STRING_MAX_LENGTH];
  int16_t x;
  int16_t y;
  uint8_t scale;
  int16_t speed;
  const char* matrixChars;
  uint8_t numMatrixChars;
  uint8_t maxScale;

  Graphics2DPrint* gfx;

  void draw() {
    gfx->setTextSize(scale);
    gfx->setTextLeftAligned();
    gfx->setTextTopAligned();

    for (uint8_t i = 0; i < length; i++) {
      gfx->setTextCursor(x, y + gfx->getTextOfsetRows(i));
      gfx->print(s[i]);
    }

    y += speed;

    if (random(100) > 75) {
      s[random(length)] = matrixChars[random(numMatrixChars)];
    }

    if (y > gfx->getHeight()) {
      randomize();
    }
  }
};

// TODO: use an dim colors correctly
class AnimMatrix {
 public:
  AnimMatrix(Graphics2DPrint* gfx, const char* chars = "GATC", uint8_t charCount = 4, uint8_t stringCount = 32,
             uint8_t maxScale = 3, uint16_t fgColor = rgb565(0, 255, 0), uint16_t maskColor = rgb565(255, 0, 0)) {
    this->chars = chars;
    this->stringCount = stringCount;
    this->maxScale = maxScale;
    this->fgColor = fgColor;
    this->maskColor = maskColor;

    matrixStrings = new MatrixString*[stringCount];

    for (uint8_t i = 0; i < stringCount; i++) {
      matrixStrings[i] = new MatrixString(gfx, chars, charCount, maxScale);
    }
  }

  void loop(Graphics2DPrint* gfx) {
    gfx->enableMask(maskColor);
    // draw back to front
    for (uint8_t c = 1; c <= maxScale; c++) {
      gfx->setTextColor(rgb565(0, 32 + c * 64, 0), rgb565(255, 0, 0));
      for (uint8_t i = 0; i < stringCount; i++) {
        if (matrixStrings[i]->scale == c) {
          matrixStrings[i]->draw();
        }
      }
    }
  }

  MatrixString** matrixStrings;
  const char* chars;
  uint8_t stringCount;
  uint8_t maxScale;
  uint16_t fgColor;
  uint16_t maskColor;
};

#endif
