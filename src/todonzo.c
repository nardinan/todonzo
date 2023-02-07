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
#define OK 0
#define KO 1
#define SAVE_REQUIRED 2
#include <ctype.h>
#include "reminder.h"
static int p_todonzo_parse_time_offset(const char *delta_time, const char *fixed_time, time_t *final_timestamp) {
  time_t current_timestamp = time(NULL);
  struct tm *current_time_definition = localtime(&current_timestamp);
  int result = OK;
  const struct {
    const char *short_extension, *full_extension, *alternate_full_extensions;
    int multiplier, *entry;
  } delta_time_extensions[] = {{"m", "min",  "mins",  1, &(current_time_definition->tm_min)},
                               {"h", "hour", "hours", 1, &(current_time_definition->tm_hour)},
                               {"d", "day",  "days",  1, &(current_time_definition->tm_mday)},
                               {"w", "week", "weeks", 7, &(current_time_definition->tm_mday)},
                               {NULL, NULL,  0}};
  if (delta_time) {
    int multiplier = 0;
    ++delta_time;
    while ((*delta_time) && (isdigit(*delta_time))) {
      multiplier = (multiplier * 10) + (*delta_time - '0');
      ++delta_time;
    }
    if ((multiplier > 0) && (isascii(*delta_time)))
      for (unsigned int index_extension = 0; (delta_time_extensions[index_extension].short_extension); ++index_extension)
        if ((strcmp(delta_time, delta_time_extensions[index_extension].short_extension) == 0) ||
            (strcmp(delta_time, delta_time_extensions[index_extension].full_extension) == 0) ||
            (strcmp(delta_time, delta_time_extensions[index_extension].alternate_full_extensions) == 0)) {
          *(delta_time_extensions[index_extension].entry) += (delta_time_extensions[index_extension].multiplier * multiplier);
          break;
        }
  }
  if (fixed_time) {
    int *filling_entry = &(current_time_definition->tm_hour);
    ++fixed_time;
    current_time_definition->tm_hour = 0;
    current_time_definition->tm_min = 0;
    while (*fixed_time) {
      if (isdigit(*fixed_time))
        *filling_entry = (*filling_entry * 10) + (*fixed_time - '0');
      else if (filling_entry != &(current_time_definition->tm_min))
        filling_entry = &(current_time_definition->tm_min);
      ++fixed_time;
    }
    if ((current_time_definition->tm_hour < 0) || (current_time_definition->tm_hour > 23) || (current_time_definition->tm_min < 0) ||
        (current_time_definition->tm_min > 59)) {
      fprintf(stderr,
        "Unable to parse correctly the fixed time (your time cannot be %02d:%02d, this is out of range)\n",
        current_time_definition->tm_hour,
        current_time_definition->tm_min);
      result = KO;
    }
  }
  if (result == OK)
    *final_timestamp = mktime(current_time_definition);
  return result;
}
int f_todonzo_add(int argc, char *argv[], s_reminder *reminders) {
  int result = OK;
  char *title = NULL, *description = NULL, *delta_time = NULL, *fixed_time = NULL;
  for (unsigned int index_argument = 1; (index_argument < argc) && (result == OK); ++index_argument) {
    if (title) {
      switch (*argv[index_argument]) {
        case '+':
          if (delta_time) {
            fprintf(stderr, "Parameter delta time cannot be re-defined (was already defined here: '%s')\n", delta_time);
            result = KO;
          } else
            delta_time = argv[index_argument];
          break;
        case '@':
          if (fixed_time) {
            fprintf(stderr, "Parameter fixed time cannot be re-defined (was already defined here: '%s')\n", fixed_time);
            result = KO;
          } else
            fixed_time = argv[index_argument];
          break;
        default:
          if (description) {
            fprintf(stderr, "Additional parameter is unknown and cannot be associated to anything else (parameter '%s')\n", argv[index_argument]);
            result = KO;
          } else
            description = argv[index_argument];
      }
    } else
      title = argv[index_argument];
  }
  if (result == OK) {
    if ((title) && ((delta_time) || (fixed_time))) {
      time_t expiration_timestamp;
      if ((result = p_todonzo_parse_time_offset(delta_time, fixed_time, &expiration_timestamp)) == OK) {
        char icon_file_path[PATH_MAX];
        f_application_get_icon(icon_file_path, PATH_MAX);
        f_reminder_add(reminders, m_reminder_UID, title, description, icon_file_path, expiration_timestamp, false);
        result = SAVE_REQUIRED;
      }
    } else
      result = KO;
  }
  return result;
}
int f_todonzo_postpone(int argc, char *argv[], s_reminder *reminders) {
  return 0;
}
int f_todonzo_show(int argc, char *argv[], s_reminder *reminders) {
  return 0;
}
int f_todonzo_run(int argc, char *argv[], s_reminder *reminders) {
  return 0;
}
typedef int (*t_todonzo_process)(int, char **, s_reminder *);
int main(int argc, char *argv[]) {
  int result = KO;
  static const struct {
    const char *short_parameter, *extend_parameter;
    t_todonzo_process function;
  } functionalities[] = {
    { "-a", "--add",  f_todonzo_add },
    { "-p", "--postpone", f_todonzo_postpone },
    { "-s", "--show", f_todonzo_show },
    { "-r", "--run", f_todonzo_run },
    { NULL, NULL, NULL }
  };
  /* syntax:
   * todonzo -a (--add) "this is the title of the notification" +N{w or weeks/d or days/h or hours/m or mins} @<hour or hour:minute>
   *  The command adds a new notification and postpones it to the next N weeks, days, hours or minutes. Additionally, you might specify a specific hour of the
   *  day (expressed in 24-hour format), or a full date when the notification has to appear.
   *
   * todonzo -p (--postpone) <UID>,<UID>,<UID>,... +N{w or weeks/d or days/h or hours/m or mins}
   *  The command postpone the last notification arrived (or a specific notification identified by its UID) by N weeks, days, hours or minutes.
   *
   * todonzo -d (--delete) <UID>,<UID>,<UID>,...
   *  The command removes one or more notifications archived either expired or not
   *
   * todonzo -s (--show)
   *  Shows, in a list, all the pending notifications, and a few of the notifications recently expired.
   *
   * todonzo -r (--run)
   *  The application checks if there is notification already expired which has not yet been processed; The system throws the notification to inform the user;
   *  If multiple notifications are expired, only one is thrown.
   */
  if (argc > 1) {
    char configuration_file_path[PATH_MAX];
    FILE *configuration_file_stream;
    s_reminder *reminders = f_array_malloc(8, sizeof(s_reminder));
    f_application_get_configuration(configuration_file_path, PATH_MAX);
    if ((configuration_file_stream = fopen(configuration_file_path, "r"))) {
      f_reminder_load(reminders, configuration_file_stream);
      fclose(configuration_file_stream);
    }
    for (unsigned int index_functionalities = 0; (functionalities[index_functionalities].short_parameter); ++index_functionalities)
      if ((strcmp(argv[1], functionalities[index_functionalities].short_parameter) == 0) ||
        ((functionalities[index_functionalities].extend_parameter) && (strcmp(argv[1], functionalities[index_functionalities].extend_parameter) == 0))) {
        result = functionalities[index_functionalities].function(argc, argv, reminders);
        break;
      }
    if (result == SAVE_REQUIRED)
      if ((configuration_file_stream = fopen(configuration_file_path, "w"))) {
        f_reminder_save(reminders, configuration_file_stream);
        fclose(configuration_file_stream);
      }
    f_array_free(reminders);
  }
  if (result == KO) {
    /* we need to print the help page */
  }
  return 0;
}