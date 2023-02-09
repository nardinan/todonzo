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
#include <math.h>
#include "reminder.h"
unsigned int m_reminder_index, m_reminder_UID;
static void p_reminder_escape_string(char *string, char forbidden_character, char replacement_character) {
  char *occurrence_pointer;
  while ((occurrence_pointer = strchr(string, forbidden_character))) {
    *occurrence_pointer = replacement_character;
    string = (occurrence_pointer + 1);
  }
}
s_reminder *f_reminder_add(s_reminder *array_reminders, unsigned int UID, const char *title, const char *description, const char *icon,
  time_t reminder_trigger_timestamp, bool processed) {
  if (title) {
    array_reminders = (s_reminder *)f_array_validate_access((void *)array_reminders, m_reminder_index);
    array_reminders[m_reminder_index].UID = UID;
    array_reminders[m_reminder_index].expiration_timestamp = reminder_trigger_timestamp;
    array_reminders[m_reminder_index].title = strdup(title);
    p_reminder_escape_string(array_reminders[m_reminder_index].title, '"', '-');
    if (icon)
      array_reminders[m_reminder_index].icon = strdup(icon);
    if (description) {
      array_reminders[m_reminder_index].description = strdup(description);
      p_reminder_escape_string(array_reminders[m_reminder_index].description, '"', '-');
    }
    array_reminders[m_reminder_index].processed = processed;
    array_reminders[m_reminder_index].initialized = true;
    if (m_reminder_index > 0) {
      int index_pivot = (m_reminder_index - 1); /* (index_pivot + 1) tells at which position the node should be moved */
      while ((index_pivot >= 0) &&
          (array_reminders[m_reminder_index].expiration_timestamp >= array_reminders[index_pivot].expiration_timestamp))
        --index_pivot;
      if ((index_pivot + 1) < m_reminder_index) {
        s_reminder temporary_holder;
        memcpy(&temporary_holder, &(array_reminders[m_reminder_index]), sizeof(s_reminder));
        memmove(&(array_reminders[index_pivot + 2]), &(array_reminders[index_pivot + 1]), (sizeof(s_reminder) * (m_reminder_index - (index_pivot + 1))));
        memcpy(&(array_reminders[index_pivot + 1]), &temporary_holder, sizeof(s_reminder));
      }
    }
    if (array_reminders[m_reminder_index].UID >= m_reminder_UID)
      m_reminder_UID = (array_reminders[m_reminder_index].UID + 1);
    ++m_reminder_index;
  }
  return array_reminders;
}
void f_reminder_save(s_reminder *array_reminders, FILE *stream) {
  if (stream)
    for (unsigned int index = 0; index < d_array_size(array_reminders); ++index)
      if (array_reminders[index].initialized)
        fprintf(stream,
          "{%lu,%lu,%lu},%u,%ld,%d,\"%s\",\"%s\",\"%s\"\n",
          ((array_reminders[index].icon) ? strlen(array_reminders[index].icon) : 0),
          ((array_reminders[index].title) ? strlen(array_reminders[index].title) : 0),
          ((array_reminders[index].description) ? strlen(array_reminders[index].description) : 0),
          array_reminders[index].UID,
          array_reminders[index].expiration_timestamp,
          ((array_reminders[index].processed) ? 1 : 0),
          ((array_reminders[index].icon) ? array_reminders[index].icon : ""),
          ((array_reminders[index].title) ? array_reminders[index].title : ""),
          ((array_reminders[index].description) ? array_reminders[index].description : ""));
}
void f_reminder_human_readable_output(s_reminder *array_reminders, bool show_expired, FILE *stream) {
  if (stream) {
    unsigned int UID_digits = floor(log10(m_reminder_UID) + 1);
    for (int index = (d_array_size(array_reminders) - 1); index >= 0; --index)
      if ((array_reminders[index].initialized) && ((show_expired) || (!array_reminders[index].processed))) {
        struct tm *expiration_timestamp_definition = localtime(&(array_reminders[index].expiration_timestamp));
        fprintf(stream,
          "%s[%*d] \"%s\" set for %02d/%02d/%04d @ %02d:%02d %s%s\n",
          ((array_reminders[index].processed) ? d_notification_color_yellow : ""),
          UID_digits,
          array_reminders[index].UID,
          array_reminders[index].title,
          expiration_timestamp_definition->tm_mday,
          (expiration_timestamp_definition->tm_mon + 1),
          (expiration_timestamp_definition->tm_year + 1900),
          expiration_timestamp_definition->tm_hour,
          expiration_timestamp_definition->tm_min,
          ((array_reminders[index].processed) ? "(expired)" : ""),
          d_notification_reset);
      }
  }
}
s_reminder *f_reminder_load(s_reminder *array_reminders, FILE *stream) {
  char *stream_line_buffer = NULL;
  size_t stream_line_length = 0;
  while (getline(&stream_line_buffer, &stream_line_length, stream) > 0) {
    unsigned int UID = m_reminder_UID;
    size_t title_length = 0, description_length = 0, icon_length = 0;
    time_t expiration_timestamp = 0;
    int processed = 0;
    if ((sscanf(stream_line_buffer, "{%lu,%lu,%lu},%u,%ld,%d", &icon_length, &title_length, &description_length, &UID,
            &expiration_timestamp, &processed) > 0) && (title_length > 0)) {
      char *stream_line_tail;
      if ((stream_line_tail = strchr(stream_line_buffer, '"'))) {
        char title[(title_length + 1)], description[(description_length + 1)], icon[(icon_length + 1)];
        memset(title, 0, (title_length + 1));
        memset(description, 0, (description_length + 1));
        memset(icon, 0, (icon_length + 1));
        if (strlen(stream_line_tail) >= (1 + icon_length + 3 + title_length + 3 + description_length + 1)) {
          ++stream_line_tail;
          if (icon_length)
            strncpy(icon, stream_line_tail, icon_length);
          stream_line_tail += (icon_length + 3);
          if (title_length)
            strncpy(title, stream_line_tail, title_length);
          stream_line_tail += (title_length + 3);
          if (description_length)
            strncpy(description, stream_line_tail, description_length);
          array_reminders = f_reminder_add(array_reminders,
            UID,
            title,
            ((description_length) ? description : NULL),
            ((icon_length) ? icon : NULL),
            expiration_timestamp,
            ((processed) ? true : false));
        } else
          fprintf(stderr, "Tainted record '%s'; The record has been discarded\n", stream_line_buffer);
      }
    }
    memset(stream_line_buffer, 0, stream_line_length);
  }
  return array_reminders;
}
int f_reminder_delete(s_reminder *array_reminders, unsigned int *UID) {
  int result = 0;
  for (unsigned int index = 0; index < d_array_size(array_reminders); ++index)
    if ((array_reminders[index].initialized) && ((!UID) || (array_reminders[index].UID == *UID))) {
      array_reminders[index].initialized = false;
      free(array_reminders[index].title);
      if (array_reminders[index].description)
        free(array_reminders[index].description);
      if (array_reminders[index].icon)
        free(array_reminders[index].icon);
      ++result;
      break;
    }
  return result;
}
int f_reminder_process(s_reminder *array_reminders) {
  time_t current_timestamp = time(NULL);
  int result = 0;
  for (int index = (d_array_size(array_reminders) - 1); index >= 0; --index)
    if ((array_reminders[index].initialized) && (!array_reminders[index].processed))
      if (array_reminders[index].expiration_timestamp <= current_timestamp) {
        f_notification_show(array_reminders[index].title, array_reminders[index].description, array_reminders[index].icon);
        array_reminders[index].processed = true;
        ++result;
        break;
      }
  return result;
}