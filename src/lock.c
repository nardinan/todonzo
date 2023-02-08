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
#include <unistd.h>
#include "lock.h"
int f_lock_wait_availability(void) {
  char lock_file_path[PATH_MAX];
  int lock_file_stream = -1;
  f_application_get_lock(lock_file_path, PATH_MAX);
  if ((lock_file_stream = open(lock_file_path, (O_CREAT | O_RDWR), 0666)) != -1)
    if (flock(lock_file_stream, LOCK_EX) != 0) {
      close(lock_file_stream);
      lock_file_stream = -1;
    }
  return lock_file_stream;
}
void f_lock_unlock(int lock_file_stream) {
  if (lock_file_stream >= 0) {
    flock(lock_file_stream, LOCK_UN);
    close(lock_file_stream);
  }
}