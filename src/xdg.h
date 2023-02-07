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
#ifndef TODONZO_XDG_H
#define TODONZO_XDG_H
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#define d_application_name "todonzo"
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
extern char *f_xdg_get_home(char *buffer, size_t size);
extern char *f_xdg_search_data(char *buffer, size_t size, const char *filename);
extern char *f_application_get_home(char *buffer, size_t size);
extern char *f_application_get_configuration(char *buffer, size_t size);
extern char *f_application_get_icon(char *buffer, size_t size);
#endif //TODONZO_XDG_H
