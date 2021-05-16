#include <sprites/renderer.h>
#include <vector>

void Renderer::addSprite(Sprite *sprite) {
    layers[sprite->getLayer()].push_back(sprite);   
}

void Renderer::removeSprite(Sprite *sprite) {
    std::vector<Sprite *> &vec = layers[sprite->getLayer()];
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        if (*it == sprite) {
            vec.erase(it);
            break;
        }
    }

    if (vec.size() == 0) {
        layers.erase(sprite->getLayer());
    }
}

void Renderer::clearAll(bool release) {
    if (release) {
        for (auto const& x : layers) {
            //cpy our vector
            std::vector<Sprite *> vec(x.second);
            for (Sprite *spr : vec) {
                delete spr;
            }
        }
    }

    layers.clear();
}

void Renderer::draw(OswHal *hal, uint16_t fillColor) {
    hal->getCanvas()->fillScreen(fillColor);

    for (auto const& x : layers) {
        for (auto spr : x.second) {
            if (spr->getVisible()) {
                spr->draw(hal);
            }
        }
    }

    hal->requestFlush();
}