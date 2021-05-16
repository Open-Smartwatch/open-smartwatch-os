#pragma once

#include <sprites/shape_sprite.h>

class CircleSprite: public ShapeSprite {
    public:
        void setSize(uint16_t x, uint16_t y) override;
        void setRadius(uint16_t r);
        uint16_t getRadius();

        void draw(OswHal *hal) override;

    protected:
        int16_t getOffsetX() override;
        int16_t getOffsetY() override;
};