#include <sprites/circle_sprite.h>

void CircleSprite::setSize(uint16_t x, uint16_t y) {
    //Circle has equal sizes
    Sprite::setSize(x, x);
}

uint16_t CircleSprite::getRadius() {
    return Sprite::getSizeX(); 
}

void CircleSprite::setRadius(uint16_t r) {
    setSize(r * 2, r * 2);
}


int16_t CircleSprite::getOffsetX() {
    if (getAlignment() & SpriteAlignment::LEFT) {
        return -(getSizeX() / 2);
    } else if (getAlignment() & SpriteAlignment::RIGHT) {
        return (getSizeX() / 2);
    }

    return 0;
}

int16_t CircleSprite::getOffsetY() {
    if (getAlignment() & SpriteAlignment::TOP) {
        return -(getSizeY() / 2);
    } else if (getAlignment() & SpriteAlignment::BOTTOM) {
        return getSizeY() / 2;
    }

    return 0;
}

void CircleSprite::draw(OswHal *hal) {
    int16_t posX = getX() - getOffsetX(), posY = getY() - getOffsetY();
    if (getFilled()) {
        hal->getCanvas()->fillCircle(posX, posY, getSizeX() / 2, getColor());
    } else {
        hal->getCanvas()->drawCircle(posX, posY, getSizeX() / 2, getColor());
    }
}
