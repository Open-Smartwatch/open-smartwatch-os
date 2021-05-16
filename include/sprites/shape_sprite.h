#pragma once

#include <sprites/sprite.h>

class ShapeSprite: public Sprite {
    public:
        void setColor(uint16_t color565) {
            color = color565;
        }

        uint16_t getColor() {
            return color;
        }

        void setFilled(bool filled) {
            isFilled = filled;
        }

        bool getFilled() {
            return isFilled;
        }

    private:
        uint16_t color = 0xFFFF;
        bool isFilled = true;
};