#include "osw_ui_util.h"

uint16_t defaultFontXOffset(uint16_t numChars, uint16_t scale) {  // works with default font only
  return numChars * 6 * scale;
}
uint16_t defaultFontYOffset(uint16_t numRows, uint16_t scale) {  // works with default font only
  return numRows * 8 * scale;
}
