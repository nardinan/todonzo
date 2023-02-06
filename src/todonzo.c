#include <string.h>
#include <stdbool.h>
#include "reminder.h"
int main() {
  /* syntax:
   * todonzo -a (--add) "this is the title of the notification" +N{w or weeks/d or days/h or hours/m or mins} @<hour/ hour:minute / day-month hour:minute>
   *  The command adds a new notification and postpones it to the next N weeks, days, hours or minutes. Additionally, you might specify a specific hour of the
   *  day (expressed in 24-hour format), or a full date when the notification has to appear.
   *
   * todonzo -p (--postpone) <UID> +N{w or weeks/d or days/h or hours/m or mins}
   *  The command postpone the last notification arrived (or a specific notification identified by its UID) by N weeks, days, hours or minutes.
   *
   * todonzo -s (--show)
   *  Shows, in a list, all the pending notifications, and a few of the notifications recently expired.
   *
   * todonzo -r (--run)
   *  The application checks if there is notification already expired which has not yet been processed; The system throws the notification to inform the user;
   *  If multiple notifications are expired, only one is thrown.
   */
  s_reminder *reminders = f_array_malloc(8, sizeof(s_reminder));
  return 0;
}
