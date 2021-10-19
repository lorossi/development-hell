#include "terminal.h"

/* Private function. Used inside HSLtoRGB conversion. */
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

/* Private function. Returns the maximum value between two. */
double _max(double a, double b)
{
  if (a > b)
    return a;
  return b;
}

/* Private function. Returns the minimum value between two. */
double _min(double a, double b)
{
  if (a < b)
    return a;
  return b;
}

/* Private function. Returns the maximum value between tree. */
double _max_3(double a, double b, double c)
{
  return _max(_max(a, b), c);
}

/* Private function. Returns the minimum value between tree. */
double _min_3(double a, double b, double c)
{
  return _min(_min(a, b), c);
}

/* Creates a RGB struct containing the three channels.
R, G, and B are in range 0-255. */
RGB createRGBcolor(int R, int G, int B)
{
  return (RGB){.R = R, .G = G, .B = B};
}

/* Creates a HSL struct containing the three channels. 
H is in range 0-360, S and L are in range 0-100. */
HSL createHSLcolor(int H, int S, int L)
{
  return (HSL){.H = H, .S = S, .L = L};
}

/* Converts a HSL color into the RGB space. */
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

/* Converts a RGB color into the HSL space. */
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

/* Converts a color provided the hue into the RGB space. 
Saturation and value are assumed to be respectively 100 and 50 */
RGB HUEtoRGB(double hue)
{
  HSL color = createHSLcolor(hue, 100, 50);
  return HSLtoRGB(color);
}

Rectangle createRectangle(int w, int h)
{
  if (w < 0 || h < 0)
    return (Rectangle){.w = -1, .h = -1};

  return (Rectangle){.w = w, .h = h};
}

/* Clears the terminal and moves cursor to upper-left position. */
void clear_terminal()
{
  printf(CLEARALL);
  printf(MOVEHOME);
  return;
};

/* Hides the blinking cursor from the terminal, disabling also echo.
Call show_cursor() to make it visible again. */
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

/* Makes echo and terminals visible again. */
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

/* Enters terminal raw mode. */
void enter_raw_mode()
{
  struct termios raw;
  tcgetattr(STDIN_FILENO, &raw);
  raw.c_lflag &= (~(ICANON | ECHO));
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/* Leaves terminal raw mode. */
void leave_raw_move()
{
  struct termios raw;
  tcgetattr(STDIN_FILENO, &raw);
  raw.c_lflag |= ((ICANON | ECHO));
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/* Moves the cursor to the bottom of the screen. */
void move_cursor_to_bottom()
{
  Rectangle terminal_size = get_terminal_size();
  if (terminal_size.w != -1 && terminal_size.h != -1)
    move_cursor_to(0, terminal_size.h + 1);

  return;
}

/* Returns the current size of the terminal. If it's not available, returns -1. */
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

/* Moves cursor to x, y coordinates (zero-indexed). */
void move_cursor_to(int x, int y)
{
  printf(ESCAPE "[%i;%iH", y + 1, x + 1);
  return;
};

/* Resets all the previously set text styles (foreground color, background color and text modes). */
void reset_styles()
{
  reset_fg();
  reset_bg();
  reset_textmode();
  return;
}

/* Set styles for the text. Accepts a variable number of styles.
The first parameter must be the number of styles. */
void set_styles(style styles, ...)
{
  va_list v;
  va_start(v, styles);

  for (int i = 0; i < styles; i++)
  {
    printf(ESCAPE "[%im", va_arg(v, style));
  }

  va_end(v);

  return;
}

/* Sets the foreground color of the text. */
void set_fg(style color)
{
  if ((color >= 30 && color <= 39) || (color >= 90 && color <= 97))
    printf(ESCAPE "[%im", color);
  return;
}

/* Sets the background color of the text. */
void set_bg(style color)
{
  if ((color >= 40 && color <= 49) || (color >= 100 && color <= 107))
    printf(ESCAPE "[%im", color);
  return;
}

/* Sets the text mode. */
void set_textmode(style mode)
{
  if (mode >= 0 && mode <= 9)
    printf(ESCAPE "[%im", mode);
}

/* Resets the foreground color of the text. */
void reset_fg()
{
  set_fg(fg_DEFAULT);
  return;
}

/* Resets the background color of the text. */
void reset_bg()
{
  set_bg(bg_DEFAULT);
  return;
}

/* Resets the text mode. */
void reset_textmode()
{
  set_textmode(TEXT_RESET);
}

/* Sets the foreground color of the text, according to the RGB values. */
void set_fg_RGB(RGB color)
{
  printf(ESCAPE "[38;2;%i;%i;%im", color.R, color.G, color.B);
  return;
}

/* Sets the background color of the text, according to the RGB values. */
void set_bg_RGB(RGB color)
{
  printf(ESCAPE "[48;2;%i;%i;%im", color.R, color.G, color.B);
  return;
}

void set_fg_HSL(HSL color)
{
  RGB converted = HSLtoRGB(color);
  set_fg_RGB(converted);
}

void set_bg_HSL(HSL color)
{
  RGB converted = HSLtoRGB(color);
  set_bg_RGB(converted);
}

/* Writes a string at certain x,y coordinates (zero-indexed). */
void write_at(int x, int y, char *s)
{
  move_cursor_to(x, y);
  printf(s);
  return;
};

/* Erases a set number of characters at certain x,y coordinates (zero-indexed). */
void erase_at(int x, int y, int length)
{
  for (int i = 0; i < length; i++)
  {
    move_cursor_to(x + i, y);
    printf(" ");
  }
  return;
}