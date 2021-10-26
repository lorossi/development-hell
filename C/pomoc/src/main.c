#define _DEFAULT_SOURCE // for nanosleep

#include <stdio.h>
#include <time.h>
#include <signal.h>  // for signal()
#include <unistd.h>  // for nanosleep()
#include <pthread.h> // for multithread
#include <string.h>  // for strlen()
#include <stdlib.h>  // for malloc() and free() and rand()

#include "terminal.h"

/* Structs declaration */
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
  pthread_mutex_t *terminal_lock;                          // terminal lock using mutex
  Phase *current_phase;                                    // current phase of the timer
  Window *w_phase, *w_total, *w_quote, *w_info, *w_status; // displayed windows
  int loop;                                                // are the routines looping?
  int show_r, advance_r, save_r, keypress_r;               // return values of threads
  int study_phases;                                        // amount of currently studied phases
  int windows_force_reload;                                // flag to force redraw of the windows
  int phase_elapsed;                                       // total time elapsed in the current phase
  int study_elapsed;                                       // total time studied in the session
  int previous_elapsed;                                    // elapsed loaded from file
  int time_paused, frozen_elapsed;                         // flag to pause time
} Parameters;

/* Constants */

const int STUDYDURATION = 45;     // duration of the study phase, minutes
const int SHORTBREAKDURATION = 5; // duration of the short break, minutes
const int LONGBREAKDURATION = 20; // duration of the long break, minutes
const int STUDYSESSIONS = 4;      // number of study sessions before long break
const int X_BORDER = 2;
const int Y_BORDER = 1;
const int PADDING = 2;
const int BUFLEN = 250;         // length of the buffers
const int SAVEINTERVAL = 1000;  // interval between saves, msec
const int SLEEP_INTERVAL = 100; // threads sleep time, msec

#define QUOTES_PATH ".QUOTES"
#define SAVE_PATH ".SAVE"

// global variables are bad but how could I use interrupts otherwise?
volatile int sigint_called;
volatile int sigwinch_called;

/* Functions declaration */
int file_read_line(char *buffer, int max_bytes, FILE *fp);
int file_count_lines(FILE *fp);
void SIGINT_handler();
void msec_sleep(int msec);
void sec_sleep(int sec);
void init_pomodoro(Phase phases[3]);
Parameters *init_parameters(Phase *current_phase, Window *w_phase, Window *w_total, Window *w_quote, Window *w_info, Window *w_status);
void delete_parameters(Parameters *p);
Phase *set_initial_phase(Phase phases[3]);
void reset_current_time(Parameters *p);
void format_elapsed_time(int elapsed, char *buffer);
void format_local_time(char *buffer);
int get_random_quote(Window *w);
void format_date(char *buffer);
void next_phase(Parameters *p);
int save_stats(Parameters *p);
int check_save();
int load_save(Parameters *p, Phase *phases);
void toggle_all_windows(Parameters *p, int visibility);
void *beep_async(void *args);
void *show_routine(void *args);
void *advance_routine(void *args);
void *save_routine(void *args);
void *keypress_routine(void *args);
int main();

/* Code starts here */

/* Read the current line from file */
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

void SIGWINCH_handler()
{
  sigwinch_called = 1;
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

Parameters *init_parameters(Phase *current_phase, Window *w_phase, Window *w_total, Window *w_quote, Window *w_info, Window *w_status)
{
  Parameters *p = malloc(sizeof(Parameters));

  p->terminal_lock = malloc(sizeof(*(p->terminal_lock)));
  pthread_mutex_init(p->terminal_lock, NULL);

  p->current_phase = current_phase;
  p->study_phases = 0;
  p->study_elapsed = 0;
  p->windows_force_reload = 1;
  p->phase_elapsed = 0;
  p->study_elapsed = 0;
  p->w_phase = w_phase;
  p->w_total = w_total;
  p->w_quote = w_quote;
  p->w_info = w_info;
  p->w_status = w_status;
  p->show_r = 1;
  p->advance_r = 1;
  p->save_r = 1;
  p->keypress_r = 1;
  p->previous_elapsed = 0;
  p->time_paused = 1;
  p->frozen_elapsed = 0;

  return p;
}

void delete_parameters(Parameters *p)
{
  pthread_mutex_destroy(p->terminal_lock);
  free(p->terminal_lock);
  free(p);
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

/* Reset current phase time */
void reset_current_time(Parameters *p)
{
  p->current_phase->started = time(NULL);
}

/* Start counting time */
void start_time(Parameters *p)
{
  p->time_paused = 0;
}

/* Go to next phase */
void next_phase(Parameters *p)
{
  pthread_t beep_thread;
  pthread_create(&beep_thread, NULL, beep_async, p->terminal_lock);
  pthread_detach(beep_thread);

  // one phase has been completed
  p->current_phase->completed++;

  // shall the phase count toward the maximum?
  // yes
  if (p->current_phase->is_study)
  {
    p->study_phases++;
    // update the total time until now
    p->previous_elapsed += p->phase_elapsed;
  }

  // check if this phase must be repeated
  if (p->current_phase->completed >= p->current_phase->repetitions && p->current_phase->repetitions)
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

  reset_current_time(p);
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

/* Get random quote and save into window */
int get_random_quote(Window *w)
{
  char r_buffer[BUFLEN];
  int rindex, count;
  FILE *fp;

  count = 0;
  fp = fopen(QUOTES_PATH, "r");

  if (fp == NULL)
    return -1;

  // reset quotes window
  windowDeleteAllLines(w);

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
      windowAddLine(w, r_buffer);

      // find author in buffer
      for (int i = 0; i < line_end - author_start; i++)
      {
        r_buffer[i] = r_buffer[i + author_start];
      }
      r_buffer[line_end - author_start] = '\0';

      // copy author into destination
      windowAddLine(w, r_buffer);

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
  p->previous_elapsed = atoi(r_buffer);

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

  return 0;
}

/* Clears terminal and hides/shows all windows on screen */
void toggle_all_windows(Parameters *p, int visibility)
{
  clear_terminal();
  windowSetVisibility(p->w_phase, visibility);
  windowSetVisibility(p->w_total, visibility);
  windowSetVisibility(p->w_quote, visibility);
  windowSetVisibility(p->w_status, visibility);
}

/* Async beeping */
void *beep_async(void *args)
{
  pthread_mutex_t *terminal_lock = args;
  for (int i = 0; i < 5; i++)
  {
    pthread_mutex_lock(terminal_lock);
    terminal_beep();
    pthread_mutex_unlock(terminal_lock);
    msec_sleep(200);
  }
  pthread_exit(0);
}

/* Routine handling terminal output */
void *show_routine(void *args)
{
  Parameters *p = args;

  while (p->loop)
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

    // refresh
    // wait for exclusive use of terminal
    pthread_mutex_lock(p->terminal_lock);
    windowShow(p->w_phase);
    windowShow(p->w_total);
    // unlock terminal
    pthread_mutex_unlock(p->terminal_lock);

    if (p->windows_force_reload)
    {
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

      // set position of w_info
      windowAutoResize(p->w_quote); // trigger resize to get the actual width
      Position quotes_br_corner = windowGetBottomRight(p->w_quote);
      windowSetPosition(p->w_info, X_BORDER, quotes_br_corner.y);
      windowSetWidth(p->w_info, windowGetSize(p->w_quote).width);

      // set position of w_status
      windowAutoResize(p->w_info); // trigger resize to get the actual width
      Position info_br_corner = windowGetBottomRight(p->w_info);
      windowSetPosition(p->w_status, X_BORDER, info_br_corner.y);
      windowSetWidth(p->w_status, windowGetSize(p->w_quote).width);
      windowSetVisibility(p->w_status, p->time_paused);
      windowAutoResize(p->w_status);

      // wait for exclusive use of terminal
      pthread_mutex_lock(p->terminal_lock);
      // clear terminal
      clear_terminal();

      // clear all windows
      windowClear(p->w_phase);
      windowClear(p->w_total);
      windowClear(p->w_quote);
      windowClear(p->w_info);
      windowClear(p->w_status);

      // display all
      windowShow(p->w_phase);
      windowShow(p->w_total);
      windowShow(p->w_quote);
      windowShow(p->w_info);
      windowShow(p->w_status);
      // unlock terminal
      pthread_mutex_unlock(p->terminal_lock);

      // don't update again
      p->windows_force_reload = 0;
    }

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
  while (p->loop)
  {
    if (p->time_paused)
    {
      // stall elapsed time
      p->current_phase->started = time(NULL) - p->frozen_elapsed;
    }
    else
    {
      int phase_elapsed;
      // calculate seconds elapsed
      phase_elapsed = time(NULL) - p->current_phase->started;

      // second line of w_total, total time spent studying
      int total_elapsed = 0;
      // add current session to total time if it's a study session
      if (p->current_phase->is_study)
        total_elapsed = phase_elapsed;

      // update the parameters struct
      // time in the current phase
      p->phase_elapsed = phase_elapsed;
      // total time studied today, until this very moment
      p->study_elapsed = total_elapsed + p->previous_elapsed;

      if (phase_elapsed / 60 > p->current_phase->duration)
      {
        // phase has been completed
        // go to next
        next_phase(p);
        // force windows reload
        p->windows_force_reload = 1;
      }
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
  while (p->loop)
  {

    if (time(NULL) - last_save > SAVEINTERVAL / 1000)
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

  while (p->loop)
  {
    if (sigint_called)
    {
      sigint_called = 0;

      // hide all windows
      // wait for exclusive use of terminal
      pthread_mutex_lock(p->terminal_lock);
      toggle_all_windows(p, 0);
      // unlock terminal
      pthread_mutex_unlock(p->terminal_lock);

      Dialog *d;
      int ret;

      d = createDialog(X_BORDER, Y_BORDER);
      dialogSetPadding(d, 4);
      dialogSetText(d, "Exit pomodoro?", 1);
      dialogSetButtons(d, "YES", "NO");
      dialogShow(d);
      ret = dialogWaitResponse(d);
      dialogClear(d);
      deleteDialog(d);

      if (ret == 1)
      {
        p->loop = 0;
      }
      else
      {
        // show windows again
        // wait for exclusive use of terminal
        pthread_mutex_lock(p->terminal_lock);
        toggle_all_windows(p, 1);
        // unlock terminal
        pthread_mutex_unlock(p->terminal_lock);

        p->windows_force_reload = 1;
      }
    }
    else if (sigwinch_called)
    {
      sigwinch_called = 0;
      p->windows_force_reload = 1;
    }

    char key;
    // wait for exclusive use of terminal
    pthread_mutex_lock(p->terminal_lock);
    key = poll_keypress();
    // unlock terminal
    pthread_mutex_unlock(p->terminal_lock);

    if (key >= 65 && key <= 90)
      key += 32;

    if (key == 'p')
    {
      p->time_paused = !p->time_paused;

      if (p->time_paused)
      {
        // keep time of the frozed elapsed phase
        p->frozen_elapsed = (time(NULL) - p->current_phase->started);
        // add info in the opportune window
        windowDeleteAllLines(p->w_status);
        windowAddLine(p->w_status, "WARNING, TIMER IS CURRENTLY PAUSED");
      }
      // force windows refresh
      p->windows_force_reload = 1;
    }
    else if (key == 's')
    {

      // hide all windows
      // wait for exclusive use of terminal
      pthread_mutex_lock(p->terminal_lock);
      toggle_all_windows(p, 0);
      // unlock terminal
      pthread_mutex_unlock(p->terminal_lock);

      p->time_paused = 1;

      Dialog *d;
      int ret;

      d = createDialog(X_BORDER, Y_BORDER);
      dialogSetPadding(d, 4);
      dialogSetText(d, "Do you want to skip the current session?", 1);
      dialogSetButtons(d, "YES", "NO");
      dialogShow(d);
      ret = dialogWaitResponse(d);
      dialogClear(d);
      deleteDialog(d);

      if (ret)
      { // load stats from file
        next_phase(p);
      }

      // show windows again
      // wait for exclusive use of terminal
      pthread_mutex_lock(p->terminal_lock);
      clear_terminal();
      toggle_all_windows(p, 1);
      // unlock terminal
      pthread_mutex_unlock(p->terminal_lock);

      p->time_paused = 0;
      p->windows_force_reload = 1;
    }

    msec_sleep(SLEEP_INTERVAL);
  }

  p->keypress_r = 0;
  pthread_exit(0);
}

int main()
{
  // init random seed
  srand(time(NULL));
  // start

  // handle signal interrupt
  signal(SIGINT, SIGINT_handler);
  // handle terminal resize
  signal(SIGWINCH, SIGWINCH_handler);
  // setup callback flags
  sigint_called = 0;
  sigwinch_called = 0;

  // set raw mode
  enter_raw_mode();

  pthread_t show_thread, advance_thread, save_thread, keypress_thread;
  Phase phases[3], *current_phase;
  Parameters *p;
  Window *w_phase, *w_total, *w_quote, *w_info, *w_status;

  init_pomodoro(phases);
  current_phase = set_initial_phase(phases);

  // w_phase keeping track of current phase
  w_phase = createWindow(X_BORDER, Y_BORDER);
  windowSetAlignment(w_phase, 0);
  windowSetPadding(w_phase, PADDING);
  windowSetFGcolor(w_phase, fg_RED);
  // w_phase keeping track of total time
  w_total = createWindow(0, 0);
  windowSetAlignment(w_total, 0);
  windowSetPadding(w_total, PADDING);
  windowSetFGcolor(w_total, fg_BRIGHT_YELLOW);
  // w_quote with... a quote
  w_quote = createWindow(X_BORDER, Y_BORDER);
  windowSetAlignment(w_quote, 0);
  windowSetPadding(w_quote, PADDING);
  windowSetAutoWidth(w_quote, 0);
  windowSetFGcolor(w_quote, fg_BRIGHT_BLUE);
  windowSetTextStyle(w_quote, text_ITALIC);
  get_random_quote(w_quote);
  // window with info
  w_info = createWindow(0, 0);
  windowSetAlignment(w_info, 0);
  windowSetPadding(w_info, PADDING);
  windowSetAutoWidth(w_info, 0);
  windowSetFGcolor(w_info, fg_BRIGHT_GREEN);
  windowAddLine(w_info, "press S to skip, P to pause, ctrl+c to exit");
  // window with current status
  w_status = createWindow(0, 0);
  windowSetAlignment(w_status, 0);
  windowSetPadding(w_status, PADDING);
  windowSetAutoWidth(w_status, 0);
  windowSetFGcolor(w_status, fg_RED);
  windowSetTextStyle(w_status, text_BLINKING);
  windowSetVisibility(w_status, 0);

  // pack the parameters
  p = init_parameters(current_phase, w_phase, w_total, w_quote, w_info, w_status);

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
    dialogSetButtons(d, "YES", "NO");
    dialogShow(d);
    ret = dialogWaitResponse(d);
    dialogClear(d);
    deleteDialog(d);

    // load stats from file
    if (ret)
      load_save(p, phases);
  }

  //start the loop
  p->loop = 1;

  // spawn threads
  pthread_create(&show_thread, NULL, show_routine, (void *)p);
  pthread_create(&advance_thread, NULL, advance_routine, (void *)p);
  pthread_create(&save_thread, NULL, save_routine, (void *)p);
  pthread_create(&keypress_thread, NULL, keypress_routine, (void *)p);

  reset_current_time(p);
  start_time(p);
  // Main thread IDLE
  while (p->show_r || p->advance_r || p->save_r || p->keypress_r)
  {
    msec_sleep(25);
  }

  // clean up
  delete_parameters(p);
  deleteWindow(w_phase);
  deleteWindow(w_total);
  deleteWindow(w_quote);

  // reset all terminal
  exit_raw_mode();
  reset_styles();
  clear_terminal();
  show_cursor();
  move_cursor_to(0, 0);

  return 0;
}