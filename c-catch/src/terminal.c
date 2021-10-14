
#include "terminal.h"

const double PI = 3.14159265359;
const double TIME_SCL = 0.25;

Terminal set_raw()
{
  Terminal term, old;

  tcgetattr(0, &old);           /* get current terminal attributes; 0 is the file descriptor for stdin */
  term = old;                   /* save old settings */
  term.c_lflag &= ~ICANON;      /* disable canonical mode */
  term.c_lflag &= ~ECHO;        /* disable echo */
  term.c_cc[VMIN] = 0;          /* wait until at least one keystroke available */
  term.c_cc[VTIME] = 0;         /* no timeout */
  tcsetattr(0, TCSANOW, &term); /* set immediately */

  return old;
}

void unset_raw(Terminal old)
{
  tcsetattr(0, TCSANOW, &old); /* reset immediately */
}

void clear_screen()
{
  printf(MOVEHOME);
  printf(CLEARALL);
  return;
};

void hide_cursor()
{
  printf(HIDECURSOR);
  return;
}

void show_cursor()
{
  printf(SHOWCURSOR);
  return;
}

Rectangle get_size()
{
  struct winsize size;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

  if (size.ws_row > 0 && size.ws_col > 0)
    return (Rectangle){.w = size.ws_col, .h = size.ws_row};

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

void write_int_at(Position p, int i)
{
  printf(ESCAPE "[%i;%iH%i", p.y + 1, p.x + 1, i);
  return;
};

void write_at_RGB(Position p, RGB color, char *s)
{
  printf(ESCAPE "[%i;%iH" ESCAPE "[38;2;%i;%i;%im%s", p.y + 1, p.x + 1, color.R, color.G, color.B, s);
  return;
};