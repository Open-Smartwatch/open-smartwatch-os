#include "sprites/sprite.h"
#include "sprites/renderer.h"

Sprite::Sprite() {
    Renderer::getInstance().addSprite(this);
}

void Sprite::setPosition(int16_t x, int16_t y) {
    positionX = x;
    positionY = y;
}

void Sprite::setLayer(uint16_t num) {
    Renderer::getInstance().removeSprite(this);
    layer = num;
    Renderer::getInstance().addSprite(this);
}

void Sprite::setSize(uint16_t x, uint16_t y) {
    sizeX = x;
    sizeY = y;
}

void Sprite::setAlignment(int align) {
    alignment = align;
}

int Sprite::getAlignment() {
    return alignment;
}

int16_t Sprite::getOffsetX() {
    if (alignment & SpriteAlignment::CENTER_H) {
        return getSizeX() / 2;
    } else if (alignment & SpriteAlignment::RIGHT) {
        return getSizeX();
    }

    return 0;
}

int16_t Sprite::getOffsetY() {
    if (alignment & SpriteAlignment::CENTER_V) {
        return getSizeY() / 2;
    } else if (alignment & SpriteAlignment::BOTTOM) {
        return getSizeY();
    }

    return 0;
}

void Sprite::setVisible(bool visible) {
    isVisible = visible;
}

bool Sprite::getVisible() {
    return isVisible;
}

int16_t Sprite::getX() { 
    return positionX; 
}

int16_t Sprite::getY() { 
    return positionY; 
}

uint16_t Sprite::getSizeX() { 
    return sizeX; 
}

uint16_t Sprite::getSizeY() { 
    return sizeY; 
}

uint16_t Sprite::getLayer() { 
    return layer; 
}

Sprite::~Sprite() {
    Renderer::getInstance().removeSprite(this);
}