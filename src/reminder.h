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
#ifndef TODONZO_REMINDER_H
#define TODONZO_REMINDER_H
#include <time.h>
#include <stdbool.h>
#include "array.h"
#include "notification.h"
typedef struct s_reminder {
  char *title, *description, *icon;
  unsigned int UID;
  time_t expiration_timestamp;
  int processed: 1, initialized: 1;
} s_reminder;
extern unsigned int m_reminder_index, m_reminder_UID;
extern s_reminder *f_reminder_add(s_reminder *array_reminders, unsigned int UID, const char *title, const char *description, const char *icon,
  time_t reminder_trigger_timestamp, bool processed);
extern void f_reminder_save(s_reminder *array_reminders, FILE *stream);
extern s_reminder *f_reminder_load(s_reminder *array_reminders, FILE *stream);
extern void f_reminder_process(s_reminder *array_reminders);
#endif //TODONZO_REMINDER_H
