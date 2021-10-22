#define _DEFAULT_SOURCE // for nanosleep

#include <stdio.h>
#include <time.h>
#include <signal.h>  // for signal()
#include <unistd.h>  // for nanosleep()
#include <pthread.h> // for multithread
#include <string.h>  // for strlen()
#include <stdlib.h>  // for malloc() and free() and rand()

#include "terminal.h"

#define QUOTES_PATH ".QUOTES"
#define SAVE_PATH ".SAVE"

typedef struct phase
{
  char *name;
  int id;
  int duration;
  int repetitions;
  int completed;
  int is_study;
  clock_t started;
  style fg_color;
  style bg_color;
  struct phase *next;
  struct phase *next_after;
} Phase;

typedef struct parameters
{
  Phase *current_phase;                      // current phase of the timer
  Window *w_phase, *w_total, *w_quote;       // displayed windows
  int show_r, advance_r, save_r, keypress_r; // return values of threads
  int study_phases;                          // amount of currently studied phases
  int windows_force_reload;                  // flag to force redraw of the windows
  int phase_elapsed;                         // total time elapsed in the current phase
  int study_elapsed;                         // total time studied in the session
} Parameters;

const int STUDYDURATION = 45;
const int SHORTBREAKDURATION = 5;
const int LONGBREAKDURATION = 20;
const int STUDYSESSIONS = 4;
const int X_BORDER = 2;
const int Y_BORDER = 1;
const int PADDING = 2;
const int BUFLEN = 250;
const int SAVEINTERVAL = 300;
const int SLEEP_INTERVAL = 50;

volatile int loop;
volatile int sigint_called;

int file_read_line(char *buffer, int max_bytes, FILE *fp)
{
  if (fgets(buffer, BUFLEN, fp) == NULL)
    return -1;

  int len = strlen(buffer);
  // remove newline from last character
  buffer[len - 1] = '\0';
  return len;
}

/* Count the number of lines in a file. */
int file_count_lines(FILE *fp)
{
  fpos_t old_pos;
  int lines;

  // save old position
  fgetpos(fp, &old_pos);
  // reset to start
  rewind(fp);

  lines = 0;
  while (!feof(fp))
  {
    char ch = fgetc(fp);
    if (ch == '\n')
      lines++;
  }

  // return to old position
  fsetpos(fp, &old_pos);

  return lines;
}

void SIGINT_handler()
{
  sigint_called = 1;
  return;
}

/* Sleep a set amount of ms */
void msec_sleep(int msec)
{
  if (msec < 0)
    return;

  struct timespec ts;
  ts.tv_sec = msec / 1000;
  ts.tv_nsec = (msec % 1000) * 1000000;
  nanosleep(&ts, NULL);
}

/* Sleep a set amount of s */
void sec_sleep(int sec)
{
  if (sec < 0)
    return;

  struct timespec ts;
  ts.tv_sec = sec;
  ts.tv_nsec = 0;
  nanosleep(&ts, NULL);
}

/* Assign all the variables needed to run the timer */
void init_pomodoro(Phase phases[3])
{
  // create array of phases
  phases[0] = (Phase){
      .name = "study",
      .id = 0,
      .duration = STUDYDURATION,
      .repetitions = STUDYSESSIONS,
      .completed = 0,
      .started = 0,
      .is_study = 1,
      .next = phases + 1,
      .next_after = phases + 2,
      .fg_color = fg_RED,
      .bg_color = bg_DEFAULT,
  };

  phases[1] = (Phase){
      .name = "short break",
      .id = 1,
      .duration = SHORTBREAKDURATION,
      .repetitions = 0,
      .completed = 0,
      .started = 0,
      .is_study = 0,
      .next = phases,
      .fg_color = fg_GREEN,
      .bg_color = bg_DEFAULT,
  };

  phases[2] = (Phase){
      .name = "long break",
      .id = 2,
      .duration = LONGBREAKDURATION,
      .repetitions = 0,
      .completed = 0,
      .started = 0,
      .is_study = 0,
      .next = phases,
      .fg_color = fg_GREEN,
      .bg_color = bg_DEFAULT,
  };
}

Parameters *init_parameters(Phase *current_phase, Window *w_phase, Window *w_total, Window *w_quote)
{
  Parameters *p = malloc(sizeof(Parameters));
  p->current_phase = current_phase;
  p->study_phases = 0;
  p->study_elapsed = 0;
  p->windows_force_reload = 1;
  p->phase_elapsed = 0;
  p->study_elapsed = 0;
  p->w_phase = w_phase;
  p->w_total = w_total;
  p->w_quote = w_quote;
  p->show_r = 1;
  p->advance_r = 1;
  p->save_r = 1;
  p->keypress_r = 1;

  return p;
}

/* Set initial phase */
Phase *set_initial_phase(Phase phases[3])
{
  Phase *current_phase;
  // set current phase
  current_phase = phases;
  // reset current phase time
  current_phase->started = time(NULL);
  return current_phase;
}

/* Format elapsed time and save into buffer */
void format_elapsed_time(int elapsed, char *buffer)
{
  int seconds, minutes, hours;

  hours = elapsed / 3600;
  minutes = (elapsed % 3600) / 60;
  seconds = elapsed % 60;

  if (hours > 0)
  {
    sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  }
  else
  {
    sprintf(buffer, "%02d:%02d", minutes, seconds);
  }
  return;
}

/* Format local time and save into buffer */
void format_local_time(char *buffer)
{
  time_t now;
  struct tm tm;
  now = time(NULL);
  tm = *localtime(&now);
  sprintf(buffer, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  return;
}

/* Get random quote and save into buffer */
int get_random_quote(char *b_quote, char *b_author)
{
  char r_buffer[BUFLEN];

  int rindex, count;
  count = 0;

  FILE *fp;
  fp = fopen(QUOTES_PATH, "r");

  if (fp == NULL)
    return -1;

  // calculate the number of lines
  const int quotes_num = file_count_lines(fp);
  rindex = random() % quotes_num;

  while (fgets(r_buffer, BUFLEN, fp) != NULL)
  {

    if (count == rindex)
    {
      int line_end;
      // remove newline and find end
      for (int i = 0; i < BUFLEN; i++)
      {
        if (r_buffer[i] == '\n')
        {
          r_buffer[i] = '\0';
          line_end = i;
          break;
        }
      }

      // find author and remove it from string
      int author_start;
      for (int i = line_end; i >= 0; i--)
      {
        if (r_buffer[i] == '@')
        {
          r_buffer[i] = '\0';
          author_start = i + 1;
          break;
        }
      }

      // copy quote into destination
      strcpy(b_quote, r_buffer);

      // find author in buffer
      for (int i = 0; i < line_end - author_start; i++)
      {
        r_buffer[i] = r_buffer[i + author_start];
      }
      r_buffer[line_end - author_start] = '\0';

      // copy author into destination
      strcpy(b_author, r_buffer);
      break;
    }

    count++;
  }

  fclose(fp);

  return 0;
}

/* Format local date to compare savefiles and save into buffer. */
void format_date(char *buffer)
{
  time_t now;
  struct tm tm;
  now = time(NULL);
  tm = *localtime(&now);
  sprintf(buffer, "%d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

/* Save stats to file.
Structure of the save file:
- timestamp (year-month-day)
- phase elapsed
- total study elapsed
- total study phases
- current phase id
- current phase completed
- current phase started
*/
int save_stats(Parameters *p)
{
  FILE *fp;
  char w_buffer[BUFLEN];

  fp = fopen(SAVE_PATH, "w");
  if (fp == NULL)
    return -1;

  // save timestamp
  format_date(w_buffer);
  fputs(w_buffer, fp);
  fputc('\n', fp);

  // save phase elapsed
  sprintf(w_buffer, "%i\n", p->phase_elapsed);
  fputs(w_buffer, fp);

  // save total study elapsed
  sprintf(w_buffer, "%i\n", p->study_elapsed);
  fputs(w_buffer, fp);

  // save total study phases
  sprintf(w_buffer, "%i\n", p->study_phases);
  fputs(w_buffer, fp);

  // save current phase id
  sprintf(w_buffer, "%i\n", p->current_phase->id);
  fputs(w_buffer, fp);

  // save current completed phases
  sprintf(w_buffer, "%i\n", p->current_phase->completed);
  fputs(w_buffer, fp);

  // save current phase started
  sprintf(w_buffer, "%li\n", p->current_phase->started);
  fputs(w_buffer, fp);

  fclose(fp);

  return 0;
}

/* Checks if a save from today is already present */
int check_save()
{
  FILE *fp;
  char buffer[BUFLEN];
  char r_buffer[BUFLEN];

  fp = fopen(SAVE_PATH, "r");
  if (fp == NULL)
    return -1;

  // read date
  if (file_read_line(r_buffer, BUFLEN, fp) == -1)
    return -1;

  // format current date
  format_date(buffer);

  if (strcmp(buffer, r_buffer) != 0)
    return 0;

  return 1;
}
/* Loads stats from file. */
int load_save(Parameters *p, Phase *phases)
{
  FILE *fp;
  char r_buffer[BUFLEN];
  int num_buffer;

  // if the save is not from today, return
  if (check_save() == -1)
    return 0;

  fp = fopen(SAVE_PATH, "r");
  if (fp == NULL)
    return -1;

  // read date, discarding first line
  if (file_read_line(r_buffer, BUFLEN, fp) == -1)
    return -1;

  // read phase elapsed
  if (file_read_line(r_buffer, BUFLEN, fp) == -1)
    return -2;

  // update the parameters struct
  p->phase_elapsed = atoi(r_buffer);

  // read total study elapsed
  if (file_read_line(r_buffer, BUFLEN, fp) == -1)
    return -3;
  // update the parameters struct
  p->study_elapsed = atoi(r_buffer);

  // read total study phases
  if (file_read_line(r_buffer, BUFLEN, fp) == -1)
    return -4;
  // update the parameters struct
  p->study_phases = atoi(r_buffer);

  // read current phase id
  if (file_read_line(r_buffer, BUFLEN, fp) == -1)
    return -5;

  // update the parameters struct
  num_buffer = atoi(r_buffer);
  // cycle to find the correct phase
  for (int i = 0; i < 3; i++)
  {
    if (phases[i].id == num_buffer)
    {
      p->current_phase = &phases[i];
      break;
    }

    return -6;
  }

  // read current phase completed
  if (file_read_line(r_buffer, BUFLEN, fp) == -1)
    return -7;
  // update the parameters struct
  p->current_phase->completed = atoi(r_buffer);

  // does not have much sense tho
  // read current phase started
  if (file_read_line(r_buffer, BUFLEN, fp) == -1)
    return -8;
  // update the parameters struct
  p->current_phase->started = atoi(r_buffer);

  // force reload
  p->windows_force_reload = 1;

  return 0;
}

/* Routine handling terminal output */
void *show_routine(void *args)
{
  Parameters *p = args;
  while (loop)
  {
    char buffer[BUFLEN];
    char num_buffer[BUFLEN];

    // remove old lines
    windowDeleteAllLines(p->w_phase);
    windowDeleteAllLines(p->w_total);
    // update w_phase color
    windowSetFGcolor(p->w_phase, p->current_phase->fg_color);
    windowSetBGcolor(p->w_phase, p->current_phase->bg_color);

    // first line of phase window
    if (p->current_phase->repetitions > 0)
      sprintf(buffer, "current phase: %s [%i/%i]", p->current_phase->name, p->current_phase->completed + 1, p->current_phase->repetitions);
    else
      sprintf(buffer, "current phase: %s", p->current_phase->name);
    windowAddLine(p->w_phase, buffer);

    // second line of phase window
    sprintf(buffer, "phase duration: %i minutes", p->current_phase->duration);
    windowAddLine(p->w_phase, buffer);

    // format time
    format_elapsed_time(p->phase_elapsed, num_buffer);
    // third line of phase window
    sprintf(buffer, "elapsed time: %s", num_buffer);
    windowAddLine(p->w_phase, buffer);

    // first line of w_total
    sprintf(buffer, "total study sessions: %i", p->study_phases);
    windowAddLine(p->w_total, buffer);

    format_elapsed_time(p->study_elapsed, num_buffer);
    sprintf(buffer, "total time studied: %s", num_buffer);
    windowAddLine(p->w_total, buffer);

    // third line of w_total
    format_local_time(buffer);
    windowAddLine(p->w_total, buffer);

    if (p->windows_force_reload)
    {
      // load quote
      char b_quote[BUFLEN], b_author[BUFLEN];
      get_random_quote(b_quote, b_author);
      // reset quotes window
      windowDeleteAllLines(p->w_quote);
      // add quotes to window
      windowAddLine(p->w_quote, b_quote);
      windowAddLine(p->w_quote, b_author);

      // set position of w_total
      windowAutoResize(p->w_phase); // trigger resize to get the actual width
      Position phase_br_corner = windowGetBottomRight(p->w_phase);
      windowSetPosition(p->w_total, phase_br_corner.x + 1, Y_BORDER);

      // set position of w_quote
      windowAutoResize(p->w_total); // trigger resize to get the actual width
      Position total_br_corner = windowGetBottomRight(p->w_total);
      windowSetPosition(p->w_quote, X_BORDER, total_br_corner.y);
      windowSetSize(p->w_quote, total_br_corner.x - X_BORDER, 4);
      // needs to be shown only once
      windowShow(p->w_quote);
      // don't update again
      p->windows_force_reload = 0;
    }

    // show windows
    windowShow(p->w_phase);
    windowShow(p->w_total);

    move_cursor_to(0, 0);

    // idle
    msec_sleep(SLEEP_INTERVAL);
  }
  p->show_r = 0;
  pthread_exit(0);
}

/* Routine handling time */
void *advance_routine(void *args)
{
  Parameters *p = args;
  while (loop)
  {
    int phase_elapsed;
    // calculate minutes elapsed
    phase_elapsed = (time(NULL) - p->current_phase->started);

    // second line of w_total
    int total_elapsed = p->study_phases * STUDYDURATION;
    // add current session to total time if it's a study session
    if (p->current_phase->is_study)
      total_elapsed += phase_elapsed;

    // update the parameters struct
    p->phase_elapsed = phase_elapsed;
    p->study_elapsed = total_elapsed;

    if (phase_elapsed / 60 > p->current_phase->duration)
    {
      // one phase has been completed
      p->current_phase->completed++;
      // force windows reload
      p->windows_force_reload = 1;
      // shall the phase count toward the maximum?
      if (p->current_phase->is_study)
      {
        // yes
        p->study_phases++;
      }

      // check if this phase must be repeated
      if (p->current_phase->completed >= p->current_phase->repetitions && p->current_phase->repetitions > 0)
      {
        // repetitions have ended
        p->current_phase->completed = 0;
        p->current_phase = p->current_phase->next_after;
      }
      else
      {
        // no repetitions or repetitions already finished
        p->current_phase = p->current_phase->next;
      }

      // erase windows
      windowClear(p->w_phase);
      windowClear(p->w_total);

      p->current_phase->started = time(NULL);
    }

    msec_sleep(SLEEP_INTERVAL);
  }
  p->advance_r = 0;
  pthread_exit(0);
}

/* Routing handling periodic saves. */
void *save_routine(void *args)
{
  Parameters *p = args;
  time_t last_save = 0;
  while (loop)
  {

    if (time(NULL) - last_save > SAVEINTERVAL)
    {
      save_stats(p);
      last_save = time(NULL);
    }
    msec_sleep(SLEEP_INTERVAL);
  }

  p->save_r = 0;
  pthread_exit(0);
}

/* Routine handling keypresses */
void *keypress_routine(void *args)
{
  Parameters *p = args;
  while (loop)
  {
    if (sigint_called)
    {
      sigint_called = 0;
      p->w_phase->visible = 0;
      p->w_total->visible = 0;
      p->w_quote->visible = 0;

      clear_terminal();

      Dialog *d;
      int ret;

      d = createDialog(X_BORDER, Y_BORDER);
      dialogSetPadding(d, 4);
      dialogSetText(d, "Exit pomodoro?", 1);
      dialogShow(d);
      ret = dialogWaitResponse(d);
      dialogClear(d);
      deleteDialog(d);

      if (ret == 1)
      {
        loop = 0;
      }
      else
      {
        p->w_phase->visible = 1;
        p->w_total->visible = 1;
        p->w_quote->visible = 1;
        p->windows_force_reload = 1;
      }
    }

    msec_sleep(SLEEP_INTERVAL);
  }

  p->keypress_r = 0;
  pthread_exit(0);
}

int main()
{
  //start the loop
  loop = 1;
  sigint_called = 0;
  // init random seed
  srand(time(NULL));

  // handle signal interrupt
  signal(SIGINT, SIGINT_handler);
  pthread_t show_thread, advance_thread, save_thread, keypress_thread;
  Phase phases[3], *current_phase;
  Parameters *p;
  Window *w_phase, *w_total, *w_quote;

  init_pomodoro(phases);
  current_phase = set_initial_phase(phases);

  // w_phase keeping track of current phase
  w_phase = createWindow(X_BORDER, Y_BORDER);
  windowSetAlignment(w_phase, 0);
  windowSetPadding(w_phase, PADDING);
  // w_phase keeping track of total time
  w_total = createWindow(0, 0);
  windowSetAlignment(w_total, 0);
  windowSetPadding(w_total, PADDING);
  windowSetFGcolor(w_total, fg_BRIGHT_YELLOW);
  // w_quote with... a quote
  w_quote = createWindow(X_BORDER, Y_BORDER);
  windowSetAlignment(w_quote, 0);
  windowSetAutoWidth(w_quote, 0);
  windowSetPadding(w_quote, PADDING);
  windowSetFGcolor(w_quote, fg_BRIGHT_BLUE);
  windowSetTextStyle(w_quote, text_ITALIC);

  // pack the parameters
  p = init_parameters(current_phase, w_phase, w_total, w_quote);

  // prepare terminal
  clear_terminal();
  hide_cursor();

  // check if a previous file session is available
  if (check_save())
  {
    Dialog *d;
    int ret;

    d = createDialog(X_BORDER, Y_BORDER);
    dialogSetPadding(d, 4);
    dialogSetText(d, "Previous session found. Continue?", 1);
    dialogShow(d);
    ret = dialogWaitResponse(d);
    dialogClear(d);
    deleteDialog(d);

    if (ret)
      // load stats from file
      load_save(p, phases);
  }

  // spawn threads
  pthread_create(&show_thread, NULL, show_routine, (void *)p);
  pthread_create(&advance_thread, NULL, advance_routine, (void *)p);
  pthread_create(&save_thread, NULL, save_routine, (void *)p);
  pthread_create(&keypress_thread, NULL, keypress_routine, (void *)p);

  // Main thread IDLE
  while (loop || p->show_r || p->advance_r || p->save_r || p->keypress_r)
  {
    msec_sleep(25);
  }

  // clean up
  free(p);
  deleteWindow(w_phase);
  deleteWindow(w_total);
  deleteWindow(w_quote);
  // reset all terminal
  reset_styles();
  clear_terminal();
  show_cursor();
  move_cursor_to(0, 0);

  return 0;
}