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
#include <ctype.h>
#include "reminder.h"
#include "lock.h"
#define OK 0
#define KO 1
#define SAVE_REQUIRED 2
typedef int (*t_todonzo_process)(int, char **, s_reminder **);
static int p_todonzo_parse_time_offset(const char *delta_time, const char *fixed_time, time_t *reference_timestamp, time_t *final_timestamp) {
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
    if ((multiplier > 0) && (isascii(*delta_time))) {
      unsigned int index_extension = 0;
      for ( ; (delta_time_extensions[index_extension].short_extension); ++index_extension)
        if ((strcmp(delta_time, delta_time_extensions[index_extension].short_extension) == 0) ||
            (strcmp(delta_time, delta_time_extensions[index_extension].full_extension) == 0) ||
            (strcmp(delta_time, delta_time_extensions[index_extension].alternate_full_extensions) == 0)) {
          *(delta_time_extensions[index_extension].entry) += (delta_time_extensions[index_extension].multiplier * multiplier);
          break;
        }
      if (!delta_time_extensions[index_extension].short_extension) {
        fprintf(stderr, "Woops:\n\tunable to parse correctly the delta time ('%s' doesn't make any sense as it doesn't match with our time domain)\n\n",
          delta_time);
        result = KO;
      }
    } else {
      fprintf(stderr, "Woops:\n\timpossible to parse the delta time ('%s' doesn't make any sense)\n\n", delta_time);
      result = KO;
    }
  }
  if ((result == OK) && (fixed_time)) {
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
        "Woops:\n\tunable to parse correctly the fixed time (your time cannot be %02d:%02d, this is out of range)\n\n",
        current_time_definition->tm_hour,
        current_time_definition->tm_min);
      result = KO;
    }
  }
  if (result == OK) {
    if (reference_timestamp)
      *reference_timestamp = current_timestamp;
    *final_timestamp = mktime(current_time_definition);
  }
  return result;
}
int f_todonzo_add(int argc, char *argv[], s_reminder **reminders) {
  int result = OK;
  char *title = NULL, *description = NULL, *delta_time = NULL, *fixed_time = NULL;
  for (unsigned int index_argument = 0; (index_argument < argc) && (result == OK); ++index_argument)
    if (title) {
      switch (*argv[index_argument]) {
        case '+':
          if (delta_time) {
            fprintf(stderr, "Woops:\n\tparameter delta time cannot be re-defined (was already defined here: '%s')\n\n", delta_time);
            result = KO;
          } else
            delta_time = argv[index_argument];
          break;
        case '@':
          if (fixed_time) {
            fprintf(stderr, "Woops:\n\tparameter fixed time cannot be re-defined (was already defined here: '%s')\n\n", fixed_time);
            result = KO;
          } else
            fixed_time = argv[index_argument];
          break;
        default:
          if (description) {
            fprintf(stderr, "Woops:\n\tadditional parameter is unknown and cannot be associated to anything else (parameter '%s')\n\n",
              argv[index_argument]);
            result = KO;
          } else
            description = argv[index_argument];
      }
    } else
      title = argv[index_argument];
  if (result == OK) {
    if ((title) && ((delta_time) || (fixed_time))) {
      time_t expiration_timestamp;
      if ((result = p_todonzo_parse_time_offset(delta_time, fixed_time, NULL, &expiration_timestamp)) == OK) {
        char icon_file_path[PATH_MAX];
        memset(icon_file_path, 0, PATH_MAX);
        f_application_get_icon(icon_file_path, PATH_MAX);
        *reminders = f_reminder_add(*reminders, m_reminder_UID, title, description, icon_file_path, expiration_timestamp, false);
        result = SAVE_REQUIRED;
      }
    } else
      result = KO;
  }
  return result;
}
int f_todonzo_delete(int argc, char *argv[], s_reminder **reminders) {
  int result = OK;
  if (argc > 0) {
    unsigned int UID = m_reminder_UID; /* m_reminder_UID keeps the next UID which has not yet been assigned */
    char *UID_list = argv[0];
    while (*UID_list) {
      if (isdigit(*UID_list)) {
        if (UID == m_reminder_UID)
          UID = 0;
        UID = (UID * 10) + (*UID_list - '0');
      } else if (UID != m_reminder_UID) {
        if (f_reminder_delete(*reminders, &UID) > 0)
          result = SAVE_REQUIRED;
        UID = m_reminder_UID;
      }
      ++UID_list;
    }
    if (UID != m_reminder_UID)
      if (f_reminder_delete(*reminders, &UID) > 0)
        result = SAVE_REQUIRED;
  }
  return result;
}
int f_todonzo_show(int argc, char *argv[], s_reminder **reminders) {
  bool show_expired = false;
  if ((argc > 0) && ((strcmp(argv[0], "-x") == 0) || (strcmp(argv[0], "--expired") == 0)))
    show_expired = true;
  f_reminder_human_readable_output(*reminders, show_expired, stdout);
  return OK;
}
int f_todonzo_run(int argc, char *argv[], s_reminder **reminders) {
  return ((f_reminder_process(*reminders) > 0) ? SAVE_REQUIRED : OK );
}
int f_todonzo_version(int argc, char *argv[], s_reminder **reminders) {
  printf("%s %d.%d.%d\n", d_application_name, d_application_version_major, d_application_version_minor, d_application_version_patch);
  return OK;
}
int main(int argc, char *argv[]) {
  int result = KO;
  static const struct {
    const char *short_parameter, *extend_parameter;
    t_todonzo_process function;
  } functionalities[] = {{"-a", "--add",     f_todonzo_add},
                         {"-d", "--delete",  f_todonzo_delete},
                         {"-s", "--show",    f_todonzo_show},
                         {"-r", "--run",     f_todonzo_run},
                         {"-v", "--version", f_todonzo_version},
                         {NULL, NULL, NULL}};
  if (argc > 1) {
    int lock_file_stream;
    if ((lock_file_stream = f_lock_wait_availability()) >= 0) {
      char configuration_file_path[PATH_MAX];
      FILE *configuration_file_stream;
      s_reminder *reminders = f_array_malloc(8, sizeof(s_reminder));
      f_application_get_configuration(configuration_file_path, PATH_MAX);
      if ((configuration_file_stream = fopen(configuration_file_path, "r"))) {
        reminders = f_reminder_load(reminders, configuration_file_stream);
        fclose(configuration_file_stream);
      }
      for (unsigned int index_functionalities = 0; (functionalities[index_functionalities].short_parameter); ++index_functionalities)
        if ((strcmp(argv[1], functionalities[index_functionalities].short_parameter) == 0) ||
            ((functionalities[index_functionalities].extend_parameter) && (strcmp(argv[1], functionalities[index_functionalities].extend_parameter) == 0))) {
          result = functionalities[index_functionalities].function((argc - 2), &(argv[2]), &reminders);
          break;
        }
      if (result == SAVE_REQUIRED)
        if ((configuration_file_stream = fopen(configuration_file_path, "w"))) {
          f_reminder_save(reminders, configuration_file_stream);
          fclose(configuration_file_stream);
        }
      f_lock_unlock(lock_file_stream);
      f_reminder_delete(reminders, NULL);
      f_array_free(reminders);
    }
  }
  if (result == KO) {
    /* we need to print the help page */
    printf(d_notification_bold"Todonzo"d_notification_reset" - A quick 'n dirty reminder application for terminals written in C\n"
           "\n\n"
           "Usage:\n"
           "\t%1$s {-a or --add} {title} [description] [+N(w or weeks | d or days | h or hours | m or mins)] [@(hour | hour:minute)]\n"
           "\t%1$s {-d or --delete} {UID}[,UID,UID,...]\n"
           "\t%1$s {-s or --show} [-x or --expired]\n"
           "\t%1$s {-r or --run}\n"
           "\t%1$s {-h or --help}\n"
           "\t%1$s {-v or --version}\n"
           "\n\n"
           "Todonzo is the perfect companion for any busy programmer, constantly focused on a terminal writing code or\n"
           "typing commands. The system is relatively easy to use and fast to interface with any application you want \n"
           "(e.g., vim). The application uses libnotify to trigger notifications, but it can easily be interfaced with\n"
           "any notification routine you wish to use.\n"
           "\n"
           "The easiest way to use Todonzo to push a new notification is:\n"
           "\n"
           "\t%1$s -a \"Call the boss to validate the details of the USS Sulaco\" +1day\n"
           "\t\tTodonzo will notify you tomorrow at the same time\n"
           "\n"
           "\t%1$s -a \"Check progresses in the main branch\" +1week @10:30\n"
           "\t\tTodonzo will notify you next week, same weekday, at 10:30 AM.\n"
           "\n"
           "\t%1$s -a \"Discuss with the team the MU-TH-UR 6000 project\" @17\n"
           "\t\tTodonzo will notify you today at 5 PM.\n", argv[0]);
  }
  return 0;
}
