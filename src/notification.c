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
#include <fcntl.h>
#include <unistd.h>
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256 /* a default length of the hostname */
#endif
#include <sys/types.h>
#include "notification.h"
void f_notification_show(const char *title, const char *message) {
  FILE *stream_who = NULL;
  if ((stream_who = popen("who", "r"))) {
    char hostname[HOST_NAME_MAX];
    if (gethostname(hostname, HOST_NAME_MAX) == 0) {
      char *stream_line_buffer = NULL, *buffer_message = NULL, terminal[PATH_MAX], whole_path[PATH_MAX];
      size_t stream_line_length = 0;
      while (getline(&stream_line_buffer, &stream_line_length, stream_who) > 0) {
        memset(terminal, 0, PATH_MAX);
        if (((sscanf(stream_line_buffer, "%*s %s", terminal)) == 1) && (strlen(terminal) > 0)) {
          int terminal_hook_descriptor;
          memset(whole_path, 0, PATH_MAX);
          snprintf(whole_path, PATH_MAX, "/dev/%s", terminal);
          if ((terminal_hook_descriptor = open(whole_path, O_WRONLY | O_NONBLOCK)) >= 0) {
            if (!buffer_message) {
              if (message)
                asprintf(&buffer_message, d_notification_reset"\n\r"d_notification_color_gray_bold""d_application_name" on %s reminds you that:"
                  d_notification_reset"\n\r"d_notification_color_yellow_bold"%s"d_notification_reset"\n\r"d_notification_color_yellow"%s"
                  d_notification_reset"\n\r", hostname, title, message);
              else
                asprintf(&buffer_message, d_notification_reset"\n\r"d_notification_color_gray_bold""d_application_name" on %s reminds you that:"
                  d_notification_reset"\n\r"d_notification_color_yellow_bold"%s"d_notification_reset"\n\r", hostname, title);
            }
            write(terminal_hook_descriptor, buffer_message, strlen(buffer_message)); /* it is bruteforce, let's ignore the output */
            close(terminal_hook_descriptor);
          }
        }
        memset(stream_line_buffer, 0, stream_line_length);
      }
      if (stream_line_buffer)
        free(stream_line_buffer);
      if (buffer_message)
        free(buffer_message);
    }
    pclose(stream_who);
  }
}

