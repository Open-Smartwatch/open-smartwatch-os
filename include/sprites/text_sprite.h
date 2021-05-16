#pragma once

#include <sprites/shape_sprite.h>
#include <string.h>

class TextSprite: public ShapeSprite {
    public:
        TextSprite(OswHal *hal) {
            this->hal = hal;
        }

        void setText(std::string text);
        std::string getText();

        void draw(OswHal *hal) override;

        void setFontSize(uint8_t s);

        void setSize(uint16_t x, uint16_t y) override;

    private:
        TextSprite() {}

        void computeSize();

        std::string text = "";
        uint8_t fontSize = 2;

        OswHal *hal;
};