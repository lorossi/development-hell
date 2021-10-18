// #define BLOCK // uncomment this line to get blocks instead of letters

#include <signal.h> // for signal()
#include <time.h>
#include <math.h>
#include "terminal.h"
#include "constants.h"

typedef struct
{
  double x, y;
} Position;

static volatile int loop = 1;
Rectangle size;

void SIGINT_handler()
{
  clear_terminal();
  show_cursor();
  move_cursor_to(0, 0);
  loop = 0;

  return;
}

void SIGWINCH_handler()
{
  clear_terminal();
  size = get_terminal_size();
  return;
}

double wrap_hue(double hue)
{
  while (hue <= 0)
    hue += 360;
  while (hue >= 360)
    hue -= 360;

  return hue;
}

double dist(Position a, Position b)
{

  return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

void animate_letters(double time)
{
  const double theta = time * TWO_PI * TIME_SCL;
  Position center = {
      .x = (1 + cos(theta)) / 2,
      .y = (1 + sin(theta)) / 2,
  };

  for (int y = 0; y < 26 * YREPETITIONS; y++)
  {
    for (int x = 0; x < 26 * XREPETITIONS; x++)
    {

      Position pos;
      pos.x = (x + BORDER) * SPACING;
      pos.y = y + BORDER;

      if (pos.x >= size.w || pos.y >= size.h)
        continue;

      Position rpos;
      rpos.x = x / (26.0 * XREPETITIONS);
      rpos.y = y / (26.0 * YREPETITIONS);

      double hue = dist(rpos, center) - time * 4 * TIME_SCL;
      RGB color = HUEtoRGB(wrap_hue(hue * 360));

#ifdef BLOCK
      char c = '\u2588';
#else
      const char c = 65 + ((x + y) % 26);
#endif

      move_cursor_to(pos.x, pos.y);
      set_fg_RGB(color);
      printf("%c", c);
    }
  }

  return;
}

int main()
{
  signal(SIGINT, SIGINT_handler);
  signal(SIGWINCH, SIGWINCH_handler);

  clear_terminal();
  hide_cursor();

  size = get_terminal_size();

  clock_t animation_started, frame_ended;
  double msec_elapsed = 0;

  animation_started = clock();
  while (loop)
  {
    double time = (double)(clock() - animation_started) / CLOCKS_PER_SEC;

    animate_letters(time);

    frame_ended = clock();
    while (msec_elapsed < 16.66)
    {
      msec_elapsed = (double)(clock() - frame_ended) / CLOCKS_PER_SEC * 1000;
    }
    msec_elapsed = 0;
  }

  return 0;
}