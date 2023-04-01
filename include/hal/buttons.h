#pragma once

#define NUM_BUTTONS 3

enum Button {
    // Every button must have an id in the range of 0 to NUM_BUTTONS-1
    BUTTON_SELECT = 0,
    BUTTON_UP = 1,
    BUTTON_DOWN = 2,
    // Historical reasons ↓
    BUTTON_1 = 0,
    BUTTON_2 = 2,
    BUTTON_3 = 1
};

extern const char* ButtonNames[];