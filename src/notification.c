/**
 * MIT License
 * Copyright (c) [2023] The Barfing Fox [Andrea Nardinocchi (andrea@nardinan.it)]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <SDL.h>
#include <SDL_image.h>
#include "notification.h"
void f_notification_show(const char *title, const char *message, const char *icon) {
  if (SDL_Init(SDL_INIT_VIDEO) >= 0) {
    int index = 20;
    SDL_Window *window;
    if ((window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 240,
      (SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_OPENGL)))) {
      SDL_Renderer *renderer;
      SDL_SetWindowBordered(window, SDL_FALSE);
      if ((renderer = SDL_CreateRenderer(window, -1, (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)))) {
        SDL_Texture *texture = NULL;
        SDL_Rect destination = { 0, 0, 0, 0 };
        SDL_Event event;
        if (icon) {
          if (IMG_Init((IMG_INIT_PNG | IMG_INIT_JPG)) == (IMG_INIT_PNG | IMG_INIT_JPG)) {
            SDL_Surface *image = NULL;
            if ((image = IMG_Load(icon))) {
              if ((texture = SDL_CreateTextureFromSurface(renderer, image))) {
                int width, height;
                SDL_QueryTexture(texture, NULL, NULL, &width, &height);
                destination.w = width;
                destination.h = height;
              }
              SDL_FreeSurface(image);
            }
          }
        }
        while ((index > 0)) {
          SDL_PollEvent(&event);
          if (texture) {
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, &destination);
          }
          SDL_RenderPresent(renderer);
          SDL_Delay(100);
          --index;
        }
        if (icon) {
          if (texture)
            SDL_DestroyTexture(texture);
          IMG_Quit();
        }
      }
    }
    SDL_Quit();
  }
}

