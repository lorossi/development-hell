// #define BLOCK // uncomment this line to get blocks instead of letters

#include <signal.h> // for signal()
#include <time.h>
#include "terminal.h"
#include "constants.h"

static volatile int loop = 1;
Rectangle size;

void SIGINT_handler()
{
  clear_screen();
  show_cursor();
  move_to((Position){0, 0});

  if (loop == 1)
    loop = 0;

  return;
}

void SIGWINCH_handler()
{
  clear_screen();
  size = get_size();
  return;
}

void animate_letters(double time)
{
  RelativePosition center = time_position(time);

  for (int y = 0; y < 26 * YREPETITIONS; y++)
  {

    for (int x = 0; x < 26 * XREPETITIONS; x++)
    {
      Position pos;
      pos.x = (x + BORDER) * SPACING;
      pos.y = y + BORDER;

      RelativePosition rpos;
      rpos.x = x / (26.0 * XREPETITIONS);
      rpos.y = y / (26.0 * YREPETITIONS);

#ifdef BLOCK
      write_at_colored_block(pos, rpos, center, time);
#else
      char c = 65 + ((x + y) % 26);
      write_at_colored(pos, rpos, center, time, c);
#endif
    }
  }

  return;
}

int main()
{
  signal(SIGINT, SIGINT_handler);
  signal(SIGWINCH, SIGWINCH_handler);

  clear_screen();
  hide_cursor();

  size = get_size();

  clock_t animation_started, frame_ended;
  double msec_elapsed = 0;

  animation_started = clock();
  while (loop)
  {
    double time = (double)(clock() - animation_started) / CLOCKS_PER_SEC;
    animate_letters(time);

    move_to((Position){0, size.h});

    frame_ended = clock();
    while (msec_elapsed < 16.66)
    {
      msec_elapsed = (double)(clock() - frame_ended) / CLOCKS_PER_SEC * 1000;
    }
    msec_elapsed = 0;
  }

  return 0;
}