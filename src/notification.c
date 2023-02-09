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
#include <stdio.h>
#include <utmp.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "notification.h"
void f_notification_show(const char *title, const char *message, const char *icon) {
  NotifyNotification *notification;
  notify_init(d_application_name);
  if ((notification = notify_notification_new(title, message, icon))) {
    notify_notification_set_urgency(notification, NOTIFY_URGENCY_CRITICAL);
    notify_notification_set_timeout(notification, NOTIFY_EXPIRES_NEVER);
    notify_notification_show(notification, NULL);
  }
}
int f_notification_show_terminal(const char *title, const char *message) {
  char *message_buffer;
  int result = 0;
  if (asprintf(&message_buffer, "\n\n"d_application_name" says:\n"d_notification_bold"%s\n"d_notification_reset""
      d_notification_italic"%s\n\n"d_notification_reset, title, message) >= 0) {
    size_t message_buffer_length = strlen(message_buffer);
    char *user_reference;
    if ((user_reference = getenv("USER"))) {
      struct utmp *session;
      setutent();
      while ((session = getutent()))
        if (session->ut_type == USER_PROCESS)
          if (strcmp(session->ut_user, user_reference) == 0) {
            char terminal_file_path[PATH_MAX];
            int terminal_file_stream;
            snprintf(terminal_file_path, PATH_MAX, "/dev/%s", session->ut_line);
            if ((terminal_file_stream = open(terminal_file_path, O_RDWR)) >= 0) {
              lseek(terminal_file_stream, 1, SEEK_SET);
              write(terminal_file_stream, message_buffer, message_buffer_length);
              close(terminal_file_stream);
              ++result;
            }
          }
    }
    free(message_buffer);
  }
  return result;
}