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
#ifndef TODONZO_NOTIFICATION_H
#define TODONZO_NOTIFICATION_H
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "xdg.h"
#define d_notification_color_yellow       "\x1b[33m"
#define d_notification_color_yellow_bold  "\x1b[33m\x1b[1m"
#define d_notification_color_gray         "\x1b[37m"
#define d_notification_color_gray_bold    "\x1b[37m\x1b[1m"
#define d_notification_color_white        "\x1b[97m"
#define d_notification_color_white_bold   "\x1b[97m\x1b[1m"
#define d_notification_italic             "\x1b[3m"
#define d_notification_bold               "\x1b[1m"
#define d_notification_reset              "\x1b[0m"
extern bool f_notification_show(const char *title, const char *message);
#endif //TODONZO_NOTIFICATION_H
