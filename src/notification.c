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
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256 /* a default length of the hostname */
#endif
#include "notification.h"
bool p_notification_show_write(const char *whole_path, const char *whole_message) {
  bool result = false;
  DIR *dev_directory;
  if ((dev_directory = opendir(whole_path))) {
    char new_whole_path[PATH_MAX];
    struct dirent *entry;
    while ((entry = readdir(dev_directory))) {
      memset(new_whole_path, 0, PATH_MAX);
      snprintf(new_whole_path, PATH_MAX, "%s/%s", whole_path, entry->d_name);
      if (entry->d_type == DT_DIR) {
        if (entry->d_name[0] != '.')
          if (p_notification_show_write(new_whole_path, whole_message))
            result = true;
      } else if ((strstr(new_whole_path, "tty")) || (strstr(new_whole_path, "pts"))) {
        int terminal_hook_descriptor;
        if ((terminal_hook_descriptor = open(new_whole_path, O_WRONLY | O_NONBLOCK)) >= 0) {
          if (write(terminal_hook_descriptor, whole_message, strlen(whole_message)) > 0) {
            printf("notification '%s' has been succesfully delivered to %s", whole_message, new_whole_path);
            result = true;
          }
          close(terminal_hook_descriptor);
        }
      }
    }
    closedir(dev_directory);
  }
  return result;
}
bool f_notification_show(const char *title, const char *message) {
  bool result = false;
  char hostname[HOST_NAME_MAX];
  if (gethostname(hostname, HOST_NAME_MAX) == 0) {
    char *buffer_message = NULL;
    if (message)
      asprintf(&buffer_message, d_notification_reset"\n\r"d_notification_color_gray_bold""d_application_name" on %s reminds you that:"
        d_notification_reset"\n\r"d_notification_color_yellow_bold"%s"d_notification_reset"\n\r"d_notification_color_yellow"%s"
        d_notification_reset"\n\r", hostname, title, message);
    else
      asprintf(&buffer_message, d_notification_reset"\n\r"d_notification_color_gray_bold""d_application_name" on %s reminds you that:"
        d_notification_reset"\n\r"d_notification_color_yellow_bold"%s"d_notification_reset"\n\r", hostname, title);
    result = p_notification_show_write("/dev", buffer_message);
    if (buffer_message)
      free(buffer_message);
  }
  return result;
}

