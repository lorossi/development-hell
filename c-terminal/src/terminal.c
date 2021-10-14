#include "terminal.h"
#include "colors.h"

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

  return;
}

void show_cursor()
{
  // turn on echo
  struct termios term;
  tcgetattr(0, &term);
  term.c_lflag |= ECHO;
  tcsetattr(0, 0, &term);

  printf(SHOWCURSOR);

  return;
}

Rectangle get_size()
{
  struct winsize size;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

  if (size.ws_row > 0 && size.ws_col > 0)
  {
    return (Rectangle){.w = size.ws_col, .h = size.ws_row};
  }

  return (Rectangle){.w = -1, .h = -1};
};

void move_to(Position p)
{
  write_at(p, '\0');

  return;
};

void write_at(Position p, char *s)
{
  printf(ESCAPE "[%i;%iH%s", p.y + 1, p.x + 1, s);

  return;
};

void write_at_RGB(Position p, RGB color, char *s)
{
  printf(ESCAPE "[%i;%iH" ESCAPE "[38;2;%i;%i;%im%s", p.y + 1, p.x + 1, color.R, color.G, color.B, s);

  return;
};