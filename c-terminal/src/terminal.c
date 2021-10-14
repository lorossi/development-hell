#include "terminal.h"

Rectangle createRectangle(int w, int h)
{
  if (w < 0 || h < 0)
    return (Rectangle){.w = -1, .h = -1};

  return (Rectangle){.w = w, .h = h};
}

void clear_terminal()
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

Rectangle get_terminal_size()
{
  struct winsize size;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

  if (size.ws_row > 0 && size.ws_col > 0)
  {
    return createRectangle(size.ws_col, size.ws_row);
  }

  return createRectangle(-1, -1);
};

void move_cursor_to(int x, int y)
{
  printf(ESCAPE "[%i;%iH", y + 1, x + 1);
  return;
};

void reset_styles()
{
  reset_fg();
  reset_bg();
  reset_textmode();
  return;
}

void set_styles(style styles, ...)
{
  va_list v;
  va_start(v, styles);

  for (int i = 0; i < styles; i++)
    printf(ESCAPE "[%im", va_arg(v, style));

  va_end(v);
  return;
}

void set_fg(style color)
{
  printf(ESCAPE "[%im", color);
  return;
}

void set_bg(style color)
{
  printf(ESCAPE "[%im", color);
  return;
}

void set_textmode(style mode)
{
  printf(ESCAPE "[%im", mode);
}

void reset_fg()
{
  set_fg(fg_DEFAULT);
  return;
}

void reset_bg()
{
  set_bg(bg_DEFAULT);
  return;
}

void reset_textmode()
{
  set_textmode(TEXT_RESET);
}

void set_fg_RGB(RGB color)
{
  printf(ESCAPE "[38;2;%i;%i;%im", color.R, color.G, color.B);
  return;
}

void set_bg_RGB(RGB color)
{
  printf(ESCAPE "[34;2;%i;%i;%im", color.R, color.G, color.B);
  return;
}

void write_at_RGB(int x, int y, RGB color, char *s)
{
  move_cursor_to(x, y);
  printf(ESCAPE "[38;2;%i;%i;%im%s", color.R, color.G, color.B, s);
  return;
};

void write_at(int x, int y, char *s)
{
  move_cursor_to(x, y);
  printf(s);
  return;
};

void erase_at(int x, int y, int length)
{
  for (int i = 0; i < length; i++)
  {
    move_cursor_to(x, y);
    printf(" ");
  }

  return;
}