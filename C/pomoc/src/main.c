#define _DEFAULT_SOURCE // for usleep

#include <stdio.h>
#include <time.h>
#include <signal.h>  // for signal()
#include <unistd.h>  // for nanosleep()
#include <pthread.h> // for multithread
#include <string.h>  // for strlen()

#include "terminal.h"

typedef struct phase
{
  char *name;
  int duration;
  int repetitions;
  int completed;
  clock_t started;
  style text_color;
  struct phase *next;
  struct phase *next_after;
} Phase;

const int STUDYDURATION = 45;
const int SHORTBREAKDURATION = 5;
const int LONGBREAKDURATION = 20;
const int STUDYSESSIONS = 4;
const int BORDER = 1;
const int WINDOWSPACING = 1;

Phase phases[3];
Phase *current_phase;
volatile int loop;

void SIGINT_handler()
{
  loop = 0;
  return;
}

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
void init_pomodoro()
{
  // create array of phases
  phases[0] = (Phase){
      .name = "Study",
      .duration = STUDYDURATION,
      .repetitions = STUDYSESSIONS,
      .completed = 0,
      .started = 0,
      .next = &phases[1],
      .next_after = &phases[2],
      .text_color = fg_RED,
  };

  phases[1] = (Phase){
      .name = "Short break",
      .duration = SHORTBREAKDURATION,
      .repetitions = 0,
      .completed = 0,
      .started = 0,
      .next = &phases[0],
      .text_color = fg_GREEN,
  };

  phases[2] = (Phase){
      .name = "Long break",
      .duration = LONGBREAKDURATION,
      .repetitions = 0,
      .completed = 0,
      .started = 0,
      .next = &phases[0],
      .text_color = fg_GREEN,
  };

  // set current phase
  current_phase = &phases[0];
  current_phase->started = time(NULL);
}

void format_time(int elapsed, char *buffer)
{
  int seconds, minutes;

  seconds = elapsed % 60;
  minutes = (elapsed - seconds * 60) % 60;
  if (minutes < 0)
    minutes = 0;

  sprintf(buffer, "%02d:%02d", minutes, seconds);
  return;
}

void *show_routine()
{
  while (loop)
  {
    int elapsed, longest;
    char elapsed_formatted[20];
    char lines[3][50];

    elapsed = time(NULL) - current_phase->started;
    format_time(elapsed, elapsed_formatted);

    if (current_phase->repetitions > 0)
      sprintf(lines[0], "current phase: %s [%i/%i]", current_phase->name, current_phase->completed + 1, current_phase->repetitions);
    else
      sprintf(lines[0], "current phase: %s", current_phase->name);

    sprintf(lines[1], "phase duration: %i minutes", current_phase->duration);
    sprintf(lines[2], "elapsed time: %s", elapsed_formatted);

    longest = 0;
    for (int i = 0; i < 3; i++)
    {
      if (strlen(lines[i]) > longest)
      {
        longest = strlen(lines[i]) + WINDOWSPACING * 2;
      }
    }
    if (longest % 2 == 1)
    {
      longest++;
    }

    for (int x = 0; x < longest + 2; x++)
    {
      for (int y = 0; y < 5; y++)
      {
        if (x == 0 || x == longest + 1)
        {
          move_cursor_to(BORDER + x, BORDER + y);
          if (y == 0 && x == 0)
            printf("\u250C"); // top left
          else if (y == 0 && x == longest + 1)
            printf("\u2510"); // top right
          else if (y == 4 && x == 0)
            printf("\u2514"); // bottom left
          else if (y == 4 && x == longest + 1)
            printf("\u2518"); // bottom right
          else
            printf("\u2502");
        }
        else if (y == 0 || y == 4)
        {
          move_cursor_to(BORDER + x, BORDER + y);
          printf("\u2500");
        }
      }
    }

    for (int i = 0; i < 3; i++)
    {
      int spacing;
      spacing = (double)(longest - strlen(lines[i])) / 2;
      set_fg(current_phase->text_color);
      move_cursor_to(spacing + BORDER + WINDOWSPACING, i + BORDER + WINDOWSPACING);
      printf("%s", lines[i]);
    }

    msec_sleep(100);
  }
  pthread_exit(0);
}

void *advance_routine()
{
  while (loop)
  {
    int elapsed_min;
    elapsed_min = (time(NULL) - current_phase->started) / 60;
    if (elapsed_min > current_phase->duration)
    {
      current_phase->completed++;
      if (current_phase->completed >= current_phase->repetitions && current_phase->repetitions > 0)
      {
        current_phase->completed = 0;
        current_phase = current_phase->next_after;
      }
      else
      {
        current_phase = current_phase->next;
      }

      // erase window
      // TODO make it better
      clear_terminal();
      current_phase->started = time(NULL);
    }

    msec_sleep(100);
  }
  pthread_exit(0);
}

int main()
{
  signal(SIGINT, SIGINT_handler);
  pthread_t show_thread, advance_thread;

  init_pomodoro();
  loop = 1;

  clear_terminal();
  hide_cursor();

  pthread_create(&show_thread, NULL, show_routine, NULL);
  pthread_create(&advance_thread, NULL, advance_routine, NULL);

  while (loop)
  {
  }

  reset_styles();
  clear_terminal();
  show_cursor();
  move_cursor_to(0, 0);

  return 0;
}