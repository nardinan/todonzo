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
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <utmpx.h>
#include <pwd.h>
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256 /* a default length of the hostname */
#endif
#include "notification.h"
static bool p_notification_is_terminal_active(const char *path) {
  bool result = false;
  struct stat st;
  if (stat(path, &st) == 0)
    if ((time(NULL) - st.st_mtime) < d_notification_IDLE_time_seconds)
      result = true;
  return result;
}
static bool p_notification_show_write(const char *whole_path, const char *whole_message) {
  bool result = false;
  struct passwd *current_user = getpwuid(getuid());
  if ((current_user) && (current_user->pw_name)) {
    setutxent();
    {
      struct utmpx *entry;
      char new_whole_path[PATH_MAX];
      while ((entry = getutxent()))
        if (entry->ut_type == USER_PROCESS)
          if (strcmp(entry->ut_user, current_user->pw_name) == 0) {
            int terminal_hook_descriptor;
            memset(new_whole_path, 0, PATH_MAX);
            snprintf(new_whole_path, PATH_MAX, "%s/%s", whole_path, entry->ut_line);
            if (p_notification_is_terminal_active(new_whole_path))
              if ((terminal_hook_descriptor = open(new_whole_path, O_WRONLY | O_NONBLOCK)) >= 0) {
                if (write(terminal_hook_descriptor, whole_message, strlen(whole_message)) > 0) {
                  printf("notification '%s' has been delivered to %s for the user '%s' with success", whole_message, new_whole_path, entry->ut_user);
                  result = true;
                }
                close(terminal_hook_descriptor);
              }
          }
    }
    endutxent();
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
