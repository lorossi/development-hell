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

/* Private function. Calculates the width of a window. */
int _windowCalculateLongestLine(Window *w)
{
  int longest = 0;

  for (int i = 0; i < w->lines; i++)
  {
    if (strlen(w->content[i]) > longest)
    {
      longest = strlen(w->content[i]);
    }
  }

  return longest;
}

/* Private function. Auto resize a window. */
void _windowAutoResize(Window *w, int longest)
{
  if (longest % 2 == 1 && w->alignment == 0)
  {
    // fix spacing in centering when the length of the longest string is odd
    longest++;
  }

  windowSetSize(w, longest + 2 * w->padding + 2, w->lines + 2);
}

/* Private function. Draw window border. */
void _windowDrawBorder(Window *w)
{
  const int width = w->size.w;
  const int height = w->size.h;

  for (int x = 0; x < width; x++)
  {
    for (int y = 0; y < height; y++)
    {
      if (y == 0)
      {
        move_cursor_to(x + w->pos.x, y + w->pos.y);

        // top line
        if (x == 0)
          printf("\u250c"); // top left corner
        else if (x == width - 1)
          printf("\u2510"); // top right corner
        else
          printf("\u2500"); // top line
      }
      else if (y == height - 1)
      {
        move_cursor_to(x + w->pos.x, y + w->pos.y);

        // bottom line
        if (x == 0)
          printf("\u2514"); // bottom left corner
        else if (x == width - 1)
          printf("\u2518"); // bottom right corner
        else
          printf("\u2500"); // bottom line
      }
      else if (x == 0 || x == width - 1)
      {
        move_cursor_to(x + w->pos.x, y + w->pos.y);

        printf("\u2502"); // vertical line
      }
    }
  }
}

/* Private function. Calculate line spacing. */
int _windowCalculateSpacing(Window *w, int current_len)
{
  if (w->alignment == -1)
    return 0;

  else if (w->alignment == 0)
    return (w->size.w - w->padding * 2 - 2 - current_len) / 2;

  else if (w->alignment == 1)
    return w->size.w - w->padding * 2 - 2 - current_len;

  return 0;
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

/* Creates a rectangle of set width and height. */
Rectangle createRectangle(int w, int h)
{
  if (w < 0 || h < 0)
    return (Rectangle){.w = -1, .h = -1};

  return (Rectangle){.w = w, .h = h};
}

/* Creates a position struct. */
Position createPosition(int x, int y)
{
  return (Position){.x = x, .y = y};
}

/* Creates a Window with set position. */
Window *createWindow(int x, int y)
{
  Position pos = createPosition(x, y);
  Rectangle size = createRectangle(1, 1);

  Window *new = malloc(sizeof(Window));

  *new = (Window){
      .pos = pos,
      .padding = 1,
      .size = size,
      .alignment = -1,
      .lines = 0,
      .auto_size = 1,
      .fg_color = fg_DEFAULT,
      .bg_color = bg_DEFAULT,
  };

  return new;
}

/* Deletes a Window. */
void deleteWindow(Window *w)
{
  free(w);
  return;
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
void exit_raw_move()
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
    move_cursor_to(0, terminal_size.h);

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

/* Awaits a keypress. A message is prompted on the terminal. */
void await_keypress(char *s)
{
  enter_raw_mode();
  if (s != NULL)
    printf("%s", s);
  getchar();
  exit_raw_move();

  return;
}

/* Awaits a enter keypress. A message is prompted on the terminal. */
void await_enter(char *s)
{
  if (s != NULL)
    printf("%s", s);
  getchar();

  return;
}

/* Sets size of a window. Size is relative to the outer border. */
void windowSetSize(Window *w, int width, int height)
{
  if (width < 0 || height < 0)
    return;

  if (width != w->size.w || height != w->size.h)
  {
    windowClear(w);
    w->size = createRectangle(width, height);
  }

  return;
}

/* Gets window size. */
Rectangle windowGetSize(Window *w)
{
  return w->size;
}

/* Sets position of a window. */
void windowSetPosition(Window *w, int x, int y)
{
  w->pos = createPosition(x, y);
  return;
}

/* Gets the position of a window. */
Position windowGetPosition(Window *w)
{
  return w->pos;
}

/* Gets the position of the bottom right corner of the window */
Position windowGetBottomRight(Window *w)
{
  return (Position){
      .x = w->pos.x + w->size.w,
      .y = w->pos.y + w->size.h,
  };
}

/* Sets windows padding. */
void windowSetPadding(Window *w, int padding)
{
  if (padding > 0)
    w->padding = padding;

  return;
}

/* Sets window alignment. Values: -1 for left, 0 for center, 1 for right. */
void windowSetAlignment(Window *w, int alignment)
{
  if (-1 <= alignment && alignment <= 1)
    w->alignment = alignment;

  return;
}

/* Sets window auto resize. Values: 1 for automatic resizing, 0 for manual sizing. */
void windowSetAutoSize(Window *w, int auto_size)
{
  if (auto_size != 0 && auto_size != 1)
    return;

  w->auto_size = auto_size;
}

/* Manually trigger window resize. */
void windowAutoResize(Window *w)
{
  int longest;
  longest = _windowCalculateLongestLine(w);
  _windowAutoResize(w, longest);
}

/* Sets window text color. */
void windowSetFGcolor(Window *w, style fg_color)
{
  w->fg_color = fg_color;
  return;
}

/* Sets window background color. */
void windowSetBGcolor(Window *w, style bg_color)
{
  w->bg_color = bg_color;
  return;
}

/* Returns the number of line of text of a window. */
int windowGetLines(Window *w)
{
  return w->lines;
}

/* Adds a line of text to the window. Returns -1 in case of error, otherwise returns the size of the line. */
int windowAddLine(Window *w, char *line)
{
  if (w->lines > MAX_LINES)
    return -1;

  strcpy(w->content[w->lines], line);
  w->lines++;

  return sizeof(w->content[w->lines - 1]);
}

/* Changes a line of text in the window. Returns -1 in case of error, otherwise returns the size of the line. */
int windowChangeLine(Window *w, char *line, int line_count)
{
  if (line_count > w->lines)
    return -1;

  strcpy(w->content[line_count], line);

  return sizeof(w->content[line_count]);
}

/* Deletes a line of text in the window. Returns -1 in case of error, otherwise returns the number of lines. */
int windowDeleteLine(Window *w, int line_count)
{
  if (line_count >= w->lines)
    return -1;

  w->lines--;

  for (int i = line_count; i < w->lines; i++)
    strcpy(w->content[i], w->content[i + 1]);

  return w->lines;
}

/* Deletes a all the lines of text in the window. Returns -1 in case of error. */
int windowDeleteAllLines(Window *w)
{
  w->lines = 0;

  return 0;
}

/* Shows a window on the terminal. */
void windowShow(Window *w)
{
  // no text, return
  if (w->lines == 0)
    return;

  // calculate longeast line
  int longest = _windowCalculateLongestLine(w);
  // auto resize window
  if (w->auto_size)
    _windowAutoResize(w, longest);

  // set styles
  set_fg(w->fg_color);
  set_bg(w->bg_color);

  // draw outer border
  _windowDrawBorder(w);

  for (int i = 0; i < w->lines; i++)
  {
    // calculate line length
    const int ll = strlen(w->content[i]);
    // calculate spacing according to alignment
    int spacing = _windowCalculateSpacing(w, ll);
    // calculate line coordinates
    const int lx = w->pos.x + w->padding + spacing + 1;
    const int ly = w->pos.y + 1 + i;
    // draw text
    move_cursor_to(lx, ly);
    printf("%s", w->content[i]);
  }
  reset_fg();

  move_cursor_to(0, 0);
}

/* Clears a window from the terminal */
void windowClear(Window *w)
{
  reset_bg();
  for (int y = 0; y < w->size.h; y++)
    erase_at(w->pos.x, y + w->pos.y - 1, w->size.w);
}