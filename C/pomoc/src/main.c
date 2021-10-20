#define _DEFAULT_SOURCE // for nanosleep

#include <stdio.h>
#include <time.h>
#include <signal.h>  // for signal()
#include <unistd.h>  // for nanosleep()
#include <pthread.h> // for multithread
#include <string.h>  // for strlen()
#include <stdlib.h>  // for malloc() and free() and rand()

#include "terminal.h"

typedef struct phase
{
  char *name;
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
  Phase *current_phase;
  Window *w_phase, *w_total, *w_quote;
  int study_phases, windows_force_reload;
} Parameters;

const int STUDYDURATION = 45;
const int SHORTBREAKDURATION = 5;
const int LONGBREAKDURATION = 20;
const int STUDYSESSIONS = 4;
const int X_BORDER = 2;
const int Y_BORDER = 1;
const int PADDING = 2;
const int QUOTES_NUM = 100;
#define QUOTES_PATH "src/include/QUOTES"

volatile int loop;

void SIGINT_handler()
{
  // stop loop
  loop = 0;
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

/* Assign all the variables needed to run the timer */
void init_pomodoro(Phase phases[3])
{
  // create array of phases
  phases[0] = (Phase){
      .name = "study",
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
  sprintf(buffer, "%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  return;
}

/* Get random quote and save into buffer */
void get_random_quote(char *b_quote, char *b_author)
{
  char buffer[250];

  int rindex, count;
  rindex = random() % QUOTES_NUM;
  count = 0;

  FILE *fp;
  fp = fopen(QUOTES_PATH, "r");

  while (fgets(buffer, 250, fp) != NULL)
  {

    if (count == rindex)
    {
      int line_end;
      // remove newline and find end
      for (int i = 0; i < 250; i++)
      {
        if (buffer[i] == '\n')
        {
          buffer[i] = '\0';
          line_end = i;
          break;
        }
      }

      // find author and remove it from string
      int author_start;
      for (int i = line_end; i >= 0; i--)
      {
        if (buffer[i] == '@')
        {
          buffer[i] = '\0';
          author_start = i + 1;
          break;
        }
      }

      // copy quote into destination
      strcpy(b_quote, buffer);

      // find author in buffer
      for (int i = 0; i < line_end - author_start; i++)
      {
        buffer[i] = buffer[i + author_start];
      }
      buffer[line_end - author_start] = '\0';

      // copy author into destination
      strcpy(b_author, buffer);
      break;
    }

    count++;
  }

  fclose(fp);
}

/* Routine handling terminal output */
void *show_routine(void *args)
{
  Parameters *p = args;
  while (loop)
  {
    int elapsed;
    char buffer[50];
    char num_buffer[20];

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
    elapsed = time(NULL) - p->current_phase->started;
    format_elapsed_time(elapsed, num_buffer);
    // third line of phase window
    sprintf(buffer, "elapsed time: %s", num_buffer);
    windowAddLine(p->w_phase, buffer);

    // first line of w_total
    sprintf(buffer, "total study sessions: %i", p->study_phases);
    windowAddLine(p->w_total, buffer);

    // second line of w_total
    int total_studied = p->study_phases * STUDYDURATION;
    // add current session to total time if it's a study session
    if (p->current_phase->is_study)
      total_studied += elapsed;

    format_elapsed_time(total_studied, num_buffer);
    sprintf(buffer, "total time studied: %s", num_buffer);
    windowAddLine(p->w_total, buffer);

    // third line of w_total
    format_local_time(buffer);
    windowAddLine(p->w_total, buffer);

    if (p->windows_force_reload)
    {
      // load quote
      char b_quote[250], b_author[250];
      get_random_quote(b_quote, b_author);
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

    // idle
    msec_sleep(50);
  }
  pthread_exit(0);
}

/* Routine handling time */
void *advance_routine(void *args)
{
  Parameters *p = args;
  while (loop)
  {
    int elapsed_min;
    // calculate minutes elapsed
    elapsed_min = (time(NULL) - p->current_phase->started) / 60;
    if (elapsed_min > p->current_phase->duration)
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

    msec_sleep(100);
  }
  pthread_exit(0);
}

int main()
{
  //start the loop
  loop = 1;
  // init random seed
  srand(time(NULL));

  // handle signal interrupt
  signal(SIGINT, SIGINT_handler);
  pthread_t show_thread, advance_thread;
  int show_return, advance_return;
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

  // pack the parameters
  p = malloc(sizeof(Parameters));
  p->current_phase = current_phase;
  p->study_phases = 0;
  p->windows_force_reload = 1;
  p->w_phase = w_phase;
  p->w_total = w_total;
  p->w_quote = w_quote;

  // prepare terminal
  clear_terminal();
  hide_cursor();
  // spawn threads
  show_return = pthread_create(&show_thread, NULL, show_routine, (void *)p);
  advance_return = pthread_create(&advance_thread, NULL, advance_routine, (void *)p);

  // IDLE
  while (loop || show_return != 0 || advance_return != 0)
  {
    msec_sleep(25);
  }

  // clean up
  free(p);
  // reset all terminal
  reset_styles();
  clear_terminal();
  show_cursor();
  move_cursor_to(0, 0);

  return 0;
}