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
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include "xdg.h"
static void p_xdg_check_and_create(const char *path) {
  struct stat directory_definition;
  if (stat(path, &directory_definition) < 0)
    mkdir(path, 0700);
}
static bool p_xdg_check_file_exists(char *buffer, size_t size, const char *path, const char *filename) {
  struct stat file_definition;
  bool result = true;
  snprintf(buffer, size, "%s/%s/%s", path, d_application_name, filename);
  if (stat(buffer, &file_definition) < 0) {
    memset(buffer, 0, size);
    result = false;
  }
  return result;
}
char *f_xdg_get_home(char *buffer, size_t size) {
  char *xdg_folder_home;
  memset(buffer, 0, size);
  if (!(xdg_folder_home = getenv("XDG_DATA_HOME")))
    snprintf(buffer, size, "%s/.local/share", getenv("HOME"));
  else
    strncpy(buffer, xdg_folder_home, size);
  buffer[size - 1] = 0;
  p_xdg_check_and_create(buffer);
  return buffer;
}
char *f_xdg_search_data(char *buffer, size_t size, const char *filename) {
  char *xdg_folders_data, *starting_pointer, *ending_pointer,
    xdg_folders_data_fallback[] = "/usr/local/share/:/usr/share/:./:../";
  if (!(xdg_folders_data = getenv("XDG_DATA_DIRS")))
    xdg_folders_data = xdg_folders_data_fallback;
  starting_pointer = xdg_folders_data;
  memset(buffer, 0, size);
  while ((ending_pointer = strchr(starting_pointer, ':'))) {
    *ending_pointer = 0;
    if (p_xdg_check_file_exists(buffer, size, starting_pointer, filename))
      break;
    starting_pointer = (ending_pointer + 1);
  }
  if (strlen(buffer) == 0)
    p_xdg_check_file_exists(buffer, size, starting_pointer, filename);
  return buffer;
}
char *f_xdg_get_runtime(char *buffer, size_t size) {
  char *xdg_folder_runtime;
  memset(buffer, 0, size);
  if (!(xdg_folder_runtime = getenv("XDG_RUNTIME_DIR")))
    strncpy(buffer, "/tmp/"d_application_name, size);
  else
    strncpy(buffer, xdg_folder_runtime, size);
  buffer[size - 1] = 0;
  p_xdg_check_and_create(buffer);
  return buffer;
}
char *f_application_get_home(char *buffer, size_t size) {
  size_t residual_size;
  f_xdg_get_home(buffer, size);
  if ((residual_size = (size - strlen(buffer))) > 0) {
    strncat(buffer, "/"d_application_name, --residual_size);
    p_xdg_check_and_create(buffer);
  }
  return buffer;
}
char *f_application_get_configuration(char *buffer, size_t size) {
  size_t residual_size;
  f_application_get_home(buffer, size);
  if ((residual_size = (size - strlen(buffer))) > 0)
    strncat(buffer, "/"d_application_name".csv", --residual_size);
  return buffer;
}
char *f_application_get_icon(char *buffer, size_t size) {
  return f_xdg_search_data(buffer, size, d_application_name".png");
}
char *f_application_get_lock(char *buffer, size_t size) {
  size_t residual_size;
  f_xdg_get_runtime(buffer, size);
  if ((residual_size = (size - strlen(buffer))) > 0)
    strncat(buffer, "/"d_application_name".lock", --residual_size);
  return buffer;
}