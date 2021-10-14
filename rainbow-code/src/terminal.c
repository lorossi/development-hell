#include "terminal.h"
#include "utils.h"

#include <stdio.h>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h>    // for STDOUT_FILENO
#include <math.h>      // for sin, cos
#include <termios.h>   // for cl_flag

const double PI = 3.14159265359;
const double TIME_SCL = 0.25;

void clear_screen()
{
  printf(MOVEHOME);
  printf(CLEARALL);
  fflush(NULL);
  return;
};

void hide_cursor()
{
  // turn off echo
  struct termios term;
  tcgetattr(0, &term);
  term.c_lflag &= ~ECHO;
  tcsetattr(0, 0, &term);

  printf(HIDECURSOR);
}

void show_cursor()
{
  // turn on echo
  struct termios term;
  tcgetattr(0, &term);
  term.c_lflag |= ECHO;
  tcsetattr(0, 0, &term);

  printf(SHOWCURSOR);
}

Rectangle get_size()
{
  struct winsize size;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

  if (size.ws_row > 0 && size.ws_col > 0)
  {
    return (Rectangle){size.ws_col, size.ws_row};
  }

  return (Rectangle){0, 0};
};

void write_at(Position p, char c)
{
  printf(ESCAPE "[%i;%iH%c", p.y + 1, p.x + 1, c);
  return;
};

void write_at_float(Position p, float f)
{
  printf(ESCAPE "[%i;%iH%f", p.y + 1, p.x + 1, f);
  return;
};

void move_to(Position p)
{
  write_at(p, '\0');
};

void write_at_colored(Position p, RelativePosition rp, RelativePosition center, double time, char c)
{
  RGB fill = posToRGB(rp, center, time);
  printf(ESCAPE "[%i;%iH" ESCAPE "[38;2;%i;%i;%im%c", p.y + 1, p.x + 1, fill.r, fill.g, fill.b, c);

  return;
};

void write_at_colored_block(Position p, RelativePosition rp, RelativePosition center, double time)
{
  RGB fill = posToRGB(rp, center, time);
  printf(ESCAPE "[%i;%iH" ESCAPE "[38;2;%i;%i;%im%s", p.y + 1, p.x + 1, fill.r, fill.g, fill.b, "\u2588");

  return;
};

RelativePosition time_position(double time)
{
  double theta;
  theta = PI * time * TIME_SCL;
  double cx, cy;
  cx = (1 + cos(theta)) / 2;
  cy = (1 + sin(theta)) / 2;
  return (RelativePosition){cx, cy};
}