#ifndef FAKE_ARDUINO_WINDOW_SDL_H
#define FAKE_ARDUINO_WINDOW_SDL_H
#include <SDL.h>
#include <SDL_image.h>

#include <fstream>

#include "gfx_2d.h"
#include "gfx_util.h"

// Source: https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface
Uint32 getpixel(SDL_Surface* surface, int x, int y) {
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

  switch (bpp) {
    case 1:
      return *p;
      break;

    case 2:
      return *(Uint16*)p;
      break;

    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        return p[0] << 16 | p[1] << 8 | p[2];
      else
        return p[0] | p[1] << 8 | p[2] << 16;
      break;

    case 4:
      return *(Uint32*)p;
      break;

    default:
      return 0; /* shouldn't happen, but avoids warnings */
  }
}
void loadPNG(Graphics2D* target, const char* path) {  //
  std::ifstream f(path);
  if (f.good()) {
    // printf("loading %s\n", path);
    SDL_Surface* image = IMG_Load(path);
    for (uint16_t x = 0; x < image->w; x++) {
      for (uint16_t y = 0; y < image->h; y++) {
        SDL_Color rgb;
        Uint32 data = getpixel(image, x, y);
        SDL_GetRGB(data, image->format, &rgb.r, &rgb.g, &rgb.b);

        target->drawPixel(x, y, rgb565(rgb.g, rgb.b, rgb.r));
      }
    }
    target->drawFrame(0, 0, target->getWidth(), target->getHeight(), 0);
  } else {
    for (uint16_t x = 0; x < target->getWidth(); x++) {
      for (uint16_t y = 0; y < target->getHeight(); y++) {
        target->drawPixel(x, y, rgb565(x, y, 0));
      }
    }
    // printf("missing %s\n", path);
  }
  f.close();
}

class SDLWindowRGB565 {
 public:
  SDLWindowRGB565(Graphics2D* graphics2d_, int width_, int height_)
      : graphics2d(graphics2d_), width(width_), height(height_) {
    // create window & renderer
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STATIC, width, height);
    textureBuffer = new uint16_t[width * height];

    // create the watch outline temp object limited to this scope:
    Graphics2D graphics2d_temp = Graphics2D(width, height, 1);
    graphics2d_temp.fillFrame(0, 0, width, height, rgb565(0, 0, 0));
  }

  SDLWindowRGB565(Graphics2D* graphics2d_, int width_, int height_, bool draw_debug)
      : graphics2d(graphics2d_), width(width_), height(height_) {
    // create window & renderer
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STATIC, width, height);
    textureBuffer = new uint16_t[width * height];
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
      // we don't emulate requestFlush() with this setup
      drawToWindow();
      SDL_PollEvent(&event);
    }
  }

  void drawWatchOverlay() {
    graphics2d->drawCircle(119, 119, 119, rgb565(255, 255, 255));
    graphics2d->fillFrame(216, 40, 10, 10, rgb565(200, 200, 200));
    graphics2d->fillFrame(216, 190, 10, 10, rgb565(200, 200, 200));
    graphics2d->fillFrame(13, 40, 10, 10, rgb565(200, 200, 200));
    graphics2d->fillFrame(13, 190, 10, 10, rgb565(200, 200, 200));
  }

  void gfx2dToTextureBuffer(void) {
    for (uint16_t x = 0; x < graphics2d->getWidth(); x++) {
      for (uint16_t y = 0; y < graphics2d->getHeight(); y++) {
        textureBuffer[x + y * graphics2d->getWidth()] = graphics2d->getPixel(x, y);
      }
    }
  }

  void drawToWindow() {
    drawWatchOverlay();
    gfx2dToTextureBuffer();

    SDL_UpdateTexture(texture, NULL, textureBuffer, width * sizeof(Uint16) /*pitch*/);
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
  uint16_t* textureBuffer = NULL;
};

#endif
