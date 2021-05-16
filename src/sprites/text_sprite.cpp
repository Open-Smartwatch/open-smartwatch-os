#include <sprites/text_sprite.h>

void TextSprite::setText(std::string t) {
    text = t;
    computeSize();
}

std::string TextSprite::getText() {
    return text;
}

void TextSprite::setFontSize(uint8_t s) {
    fontSize = s;
    computeSize();
}

void TextSprite::setSize(uint16_t x, uint16_t y) {
    //Don't allow a TextSprite's size to be set
}

void TextSprite::computeSize() {
    hal->getCanvas()->setTextSize(fontSize);
    
    int16_t x, y;
    uint16_t sizeX, sizeY;

    hal->getCanvas()->getTextBounds(text.c_str(), getX(), getY(), &x, &y, &sizeX, &sizeY);
    Sprite::setSize(sizeX, sizeY);
}

void TextSprite::draw(OswHal *hal) {
    
    hal->getCanvas()->setTextColor(getColor());
    hal->getCanvas()->setTextSize(fontSize);
    
    int16_t offsetX = getOffsetX();
    int16_t offsetY = getOffsetY();

    hal->getCanvas()->setCursor(getX() - offsetX, getY() - offsetY);
    hal->getCanvas()->print(text.c_str());
}