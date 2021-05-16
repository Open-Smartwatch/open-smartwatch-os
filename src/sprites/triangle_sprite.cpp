#include "sprites/triangle_sprite.h"
#include <math.h>

void TriangleSprite::setSize(uint16_t x, uint16_t y) {
    //Get Sidelength from height
    uint16_t sideLength = sqrt(pow((x / 2.0), 2) + pow(y, 2));

    setSideLength(sideLength, x, sideLength);
}

void TriangleSprite::setSideLength(uint16_t left, uint16_t base, uint16_t right) {
    this->left = left;
    this->right = right;

    double halfPerm = (left + base + right) / 2.0;
    double area = sqrt(halfPerm * (halfPerm - left) * (halfPerm - base) * (halfPerm - right));
    uint16_t height = (uint16_t)(2 * (area / base));

    Sprite::setSize(base, height);
}

int16_t TriangleSprite::getOffsetX() {
    if (getAlignment() & SpriteAlignment::LEFT) {
        return -(getSizeX() / 2);
    } else if (getAlignment() & SpriteAlignment::RIGHT) {
        return (getSizeX() / 2);
    }

    return 0;
}

int16_t TriangleSprite::getOffsetY() {
    if (getAlignment() & SpriteAlignment::TOP) {
        return -(getSizeY() / 2);
    } else if (getAlignment() & SpriteAlignment::BOTTOM) {
        return getSizeY() / 2;
    }

    return 0;
}

uint16_t TriangleSprite::getLeftLength() {
    return left;
}

uint16_t TriangleSprite::getRightLength() {
    return right;
}

void TriangleSprite::draw(OswHal *hal) {
    int16_t x0 = getX() - getOffsetX();
    int16_t y0 = getY() - getSizeY() / 2 - getOffsetY();

    int16_t x1 = getX() - getSizeX() / 2 - getOffsetX();
    int16_t y1 =  getY() + getSizeY() / 2 - getOffsetY();

    int16_t x2 = getX() + getSizeX() / 2 - getOffsetX();
    int16_t y2 = getY() + getSizeY() / 2 - getOffsetY();

    if (getFilled()) {
        hal->getCanvas()->fillTriangle(x0, y0, x1, y1, x2, y2, getColor());
    } else {
        hal->getCanvas()->drawTriangle(x0, y0, x1, y1, x2, y2, getColor());
    }
}