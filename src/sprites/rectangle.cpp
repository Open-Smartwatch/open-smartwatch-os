#include <sprites/rectangle_sprite.h>

uint16_t RectangleSprite::getCornerRadius() {
    return cornerRadius; 
}

void RectangleSprite::setCornerRadius(uint16_t r) {
    cornerRadius = r;
}

void RectangleSprite::draw(OswHal *hal) {
    int16_t posX = getX() - getOffsetX(), posY = getY() - getOffsetY();
    if (cornerRadius > 0) {
        if (getFilled()) {
            hal->getCanvas()->fillRoundRect(posX, posY, getSizeY(), getSizeX(), cornerRadius, getColor());
        } else {
            hal->getCanvas()->drawRoundRect(posX, posY, getSizeY(), getSizeX(), cornerRadius, getColor());
        }
    } else {
        if (getFilled()) {
            hal->getCanvas()->fillRect(posX, posY, getSizeY(), getSizeX(), getColor());
        } else {
            hal->getCanvas()->drawRect(posX, posY, getSizeY(), getSizeX(), getColor());
        }
    }
}
