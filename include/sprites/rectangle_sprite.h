#pragma once

#include <sprites/shape_sprite.h>

class RectangleSprite: public ShapeSprite {
    public:
        void setCornerRadius(uint16_t r);
        uint16_t getCornerRadius();

        void draw(OswHal *hal) override;

    private:
        uint16_t cornerRadius = 0;
};