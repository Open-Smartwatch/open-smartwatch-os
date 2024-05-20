#pragma once

#include "osw_pins.h"

/* button order is: select, down, up

.           _.---._
.          /,-"0"-.\ up
.         //   |   \\
.        ||9   o   3|D
.         \\    `. //
.   select \`-.6.-'/ down
.           `=---='
            
*/
#define BTN_NUMBER 3
#define BTN_POSX_ARRAY {24, 208, 208}
#define BTN_POSY_ARRAY {190, 190, 44}
#define BTN_POS_ISTOP_ARRAY {false, false, true}
#define BTN_POS_ISLEFT_ARRAY {true, false, false}
#define BTN_NAME_ARRAY {"SELECT", "DOWN", "UP"}
#define BTN_PIN_ARRAY {BTN_1, BTN_2, BTN_3}

enum Button {
    // Every button must have an id in the range of 0 to BTN_NUMBER-1
    BUTTON_SELECT = 0,
    BUTTON_DOWN = 1,
    BUTTON_UP = 2,
    // Historical reasons (will be removed in the future!) ↓
    BUTTON_1 = 0,
    BUTTON_2 = 1,
    BUTTON_3 = 2
};

extern const char* ButtonNames[];
