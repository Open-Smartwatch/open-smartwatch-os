#pragma once

#include <vector>
#include <osw_hal.h>
#include <sprites/sprite.h>
#include <map>

class Renderer {
    public:
        Renderer(): layers() {}

        static Renderer &getInstance() {
            static Renderer instance;
            return instance;
        }

        void addSprite(Sprite *sprite);
        void removeSprite(Sprite *sprite);
        void clearAll(bool release = false);

        void draw(OswHal *hal, uint16_t fillColor = 0);

        ~Renderer() {};
    private:
        std::map<uint16_t, std::vector<Sprite *>> layers;
};