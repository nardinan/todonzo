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
#include <SDL_ttf.h>
#include "notification.h"
void p_notification_write(SDL_Renderer *renderer, const char *message, int size_font, int x, int y, int window_width, int *last_y_position) {
  TTF_Font *font;
  char font_file_path[PATH_MAX];
  memset(font_file_path, 0, PATH_MAX);
  f_application_get_font(font_file_path, PATH_MAX);
  if (last_y_position)
    *last_y_position = y;
  if ((font = TTF_OpenFont(font_file_path, size_font))) {
    char processable_string[d_notification_maximum_string_size] = { 0 };
    size_t original_string_length = strlen(message), processable_string_length, residual_bytes;
    SDL_Rect destination_message = { x, y, 0, 0 };
    strncpy(processable_string, message, (d_notification_maximum_string_size - 1));
    processable_string_length = strlen(processable_string);
    residual_bytes = original_string_length - processable_string_length;
    while ((residual_bytes > 0) || (processable_string_length > 0)) {
      SDL_Texture *texture = NULL;
      do {
        SDL_Surface *surface = NULL;
        if (texture) {
          while ((processable_string_length > 0) && (processable_string[processable_string_length] != ' ')) {
            --processable_string_length;
            ++residual_bytes;
          }
          processable_string[processable_string_length] = 0;
          SDL_DestroyTexture(texture);
          texture = NULL;
        }
        if ((surface = TTF_RenderText_Solid(font, processable_string, (SDL_Color){255, 255, 255}))) {
          if ((texture = SDL_CreateTextureFromSurface(renderer, surface))) {
            SDL_QueryTexture(texture, NULL, NULL, &(destination_message.w), &(destination_message.h));
            if (last_y_position)
              *last_y_position = (destination_message.y + destination_message.w);
          }
          SDL_FreeSurface(surface);
        }
      } while ((texture) && ((destination_message.w > (window_width - x))));
      if (texture) {
        SDL_RenderCopy(renderer, texture, NULL, &destination_message);
        SDL_DestroyTexture(texture);
        texture = NULL;
      }
      if (residual_bytes > 0) {
        destination_message.y += destination_message.h;
        memset(processable_string, 0, d_notification_maximum_string_size);
        strncpy(processable_string, message + (original_string_length - residual_bytes), (d_notification_maximum_string_size - 1));
        processable_string_length = strlen(processable_string);
        residual_bytes = (residual_bytes - processable_string_length);
      } else
        processable_string_length = 0;
    }
    TTF_CloseFont(font);
  }
}
void f_notification_show(const char *title, const char *message, const char *icon) {
  if ((SDL_Init(SDL_INIT_VIDEO) >= 0) && (TTF_Init() >= 0)) {
    int index = 100;
    SDL_Window *window;
    if ((window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, d_notification_size_w, d_notification_size_h,
      (SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_OPENGL)))) {
      SDL_Renderer *renderer;
      SDL_SetWindowBordered(window, SDL_FALSE);
      SDL_FlashWindow(window, SDL_FLASH_UNTIL_FOCUSED);
      if ((renderer = SDL_CreateRenderer(window, -1, (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)))) {
        int renderer_width, renderer_height;
        SDL_Texture *texture = NULL;
        SDL_Rect destination_icon = { 0, 0, 0, 0 };
        SDL_Event event;
        SDL_GetRendererOutputSize(renderer, &renderer_width, &renderer_height);
        if (icon) {
          if (IMG_Init((IMG_INIT_PNG | IMG_INIT_JPG)) == (IMG_INIT_PNG | IMG_INIT_JPG)) {
            SDL_Surface *surface = NULL;
            if ((surface = IMG_Load(icon))) {
              if ((texture = SDL_CreateTextureFromSurface(renderer, surface))) {
                int width, height;
                SDL_QueryTexture(texture, NULL, NULL, &width, &height);
                destination_icon.w = width;
                destination_icon.h = height;
              }
              SDL_FreeSurface(surface);
            }
          }
        }
        while ((index > 0)) {
          int last_position_y = 130;
          SDL_PollEvent(&event);
          if (texture) {
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, &destination_icon);
          }
          if (title)
            p_notification_write(renderer, title, 36, 130, 0, renderer_width, &last_position_y);
          if (message)
            p_notification_write(renderer, message, 20, 130, last_position_y, renderer_width, NULL);
          SDL_RenderPresent(renderer);
          SDL_Delay(150);
          --index;
          if ((event.type == SDL_MOUSEBUTTONDOWN) && (event.button.button == SDL_BUTTON_LEFT))
            index = 0;
        }
        if (icon) {
          if (texture)
            SDL_DestroyTexture(texture);
          IMG_Quit();
        }
        SDL_DestroyRenderer(renderer);
      }
      SDL_DestroyWindow(window);
    }
    SDL_Quit();
  }
}

