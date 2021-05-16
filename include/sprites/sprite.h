#pragma once

#include <stdint.h>
#include <osw_hal.h>


enum SpriteAlignment {
    LEFT = 1,
    CENTER_H = 2,
    RIGHT = 4,
    TOP = 8,
    CENTER_V = 16,
    BOTTOM = 32
};


class Sprite {
    public:
        Sprite();

        virtual void setPosition(int16_t x, int16_t y);
        virtual void setSize(uint16_t x, uint16_t y);
        void setVisible(bool visible);
        bool getVisible();

        void setLayer(uint16_t layer);
        virtual void draw(OswHal *hal) {}

        void setAlignment(int align);
        int getAlignment();

        int16_t getX();
        int16_t getY();
        virtual uint16_t getSizeX();
        virtual uint16_t getSizeY();
        uint16_t getLayer();

        virtual ~Sprite();

    private:
        bool isVisible = true;
        uint16_t sizeX = 0;
        uint16_t sizeY = 0;
        int16_t positionX = 0;
        int16_t positionY = 0;
        uint16_t layer = 10;

        int alignment = (SpriteAlignment::CENTER_H | SpriteAlignment::CENTER_V);

    protected:
        virtual int16_t getOffsetX();
        virtual int16_t getOffsetY();
};