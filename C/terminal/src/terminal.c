#include "terminal.h"

double _hue_to_rgb(double p, double q, double t)
{
  if (t < 0)
    t += 1;
  if (t > 1)
    t -= 1;

  if (t < 1.0 / 6)
    return p + (q - p) * 6.0 * t;
  if (t < 1.0 / 2)
    return q;
  if (t < 2.0 / 3)
    return p + (q - p) * (2.0 / 3 - t) * 6.0;

  return p;
}

double _max(double a, double b)
{
  if (a > b)
    return a;
  return b;
}

double _min(double a, double b)
{
  if (a < b)
    return a;
  return b;
}

double _max_3(double a, double b, double c)
{
  return _max(_max(a, b), c);
}

double _min_3(double a, double b, double c)
{
  return _min(_min(a, b), c);
}

RGB createRGBcolor(int R, int G, int B)
{
  return (RGB){.R = R, .G = G, .B = B};
}

HSL createHSLcolor(int H, int S, int L)
{
  return (HSL){.H = H, .S = S, .L = L};
}

RGB HSLtoRGB(HSL color)
{
  double h, s, l;
  h = (double)color.H / 360;
  s = (double)color.S / 100;
  l = (double)color.L / 100;

  double r, g, b;
  if (s == 0)
  {
    r = l;
    g = l;
    b = l;
  }
  else
  {
    double q, p;
    q = l < 0.5 ? l * (1 + s) : l + s - l * s;
    p = 2 * l - q;

    r = _hue_to_rgb(p, q, h + 1.0 / 3);
    g = _hue_to_rgb(p, q, h);
    b = _hue_to_rgb(p, q, h - 1.0 / 3);
  }

  return createRGBcolor(r * 255, g * 255, b * 255);
}

HSL RGBtoHSL(RGB color)
{
  double r, g, b;
  r = (double)color.R / 255;
  g = (double)color.G / 255;
  b = (double)color.B / 255;

  double max, min;
  max = _max_3(r, g, b);
  min = _min_3(r, g, b);

  double h, s, l;
  l = (max + min) / 2;

  if (max == min)
  {
    h = 0;
    s = 0;
  }
  else
  {
    double d;
    d = max - min;
    s = l > 0.5 ? d / (2 - max - min) : d / (max + min);

    if (max == r)
      h = (g - b) / d + (g < b ? 6 : 0);
    else if (max == g)
      h = (b - r) / d + 2;
    else if (max == b)
      h = (r - g) / d + 4;
  }

  return createHSLcolor(h * 60, s * 100, l * 100);
}

RGB HUEtoRGB(double hue)
{
  HSL color = createHSLcolor(hue, 255, 127);
  return HSLtoRGB(color);
}

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