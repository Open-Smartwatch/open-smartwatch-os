#ifndef FAKE_ARDUINO_WINDOW_SDL_H
#define FAKE_ARDUINO_WINDOW_SDL_H
#include <SDL.h>

#include "gfx_2d.h"
#include "gfx_util.h"

class SDLWindowRGB565 {
 public:
  SDLWindowRGB565(Graphics2D* graphics2d_, int width_, int height_)
      : graphics2d(graphics2d_), width(width_), height(height_) {
    // create window & renderer
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STATIC, width, height);
    textureBuffer = new uint32_t[width * height];
  }

  ~SDLWindowRGB565() {
    delete[] textureBuffer;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  virtual void setup(){};
  virtual void loop(){};

  void run() {
    setup();
    SDL_Event event;  // Event variable
    while (!(event.type == SDL_QUIT)) {
      loop();
      drawToWindow();
      SDL_PollEvent(&event);
    }
  }

  void gfx2dToTextureBuffer(void) {
    for (uint8_t chunk; chunk < graphics2d->numChunks(); chunk++) {
      uint16_t* chunkBuffer = graphics2d->getChunk(chunk);

      for (uint8_t x = 0; x < width; x++) {
        for (uint8_t y = 0; y < graphics2d->getChunkHeight(); y++) {
          uint16_t rgb = chunkBuffer[x + y * width];
          textureBuffer[x + (chunk * graphics2d->getChunkHeight() + y) * width] =
              (rgb565_red(rgb) << 16) | (rgb565_green(rgb) << 8) | (rgb565_blue(rgb));
        }
      }
    }
  }

  void drawToWindow() {
    gfx2dToTextureBuffer();
    SDL_UpdateTexture(texture, NULL, textureBuffer, width * sizeof(Uint32) /*pitch*/);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

 private:
  Graphics2D* graphics2d;
  int height;
  int width;
  SDL_Renderer* renderer = NULL;
  SDL_Window* window = NULL;
  SDL_Texture* texture = NULL;
  uint32_t* textureBuffer = NULL;
};

#endif