#pragma once

enum Button {
    // Every button must have an id in the range of 0 to BTN_NUMBER-1
    BUTTON_SELECT = 0,
    BUTTON_UP = 1,
    BUTTON_DOWN = 2,
    // Historical reasons (will be removed in the future!) ↓
    BUTTON_1 = 0,
    BUTTON_2 = 2,
    BUTTON_3 = 1
};

extern const char* ButtonNames[];
