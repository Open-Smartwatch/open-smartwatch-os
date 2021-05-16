#pragma once

#include <sprites/shape_sprite.h>

class TriangleSprite: public ShapeSprite {
    public:
        TriangleSprite() {
            setSideLength(5, 5, 5);
        }

        void setSize(uint16_t x, uint16_t y) override;
        void setSideLength(uint16_t left, uint16_t base, uint16_t right);
        uint16_t getLeftLength();
        uint16_t getRightLength();

        void draw(OswHal *hal) override;

    private:
        uint16_t right;
        uint16_t left;

    protected:
        int16_t getOffsetX() override;
        int16_t getOffsetY() override;
};