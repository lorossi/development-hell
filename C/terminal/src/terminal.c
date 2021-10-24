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

/* Private function. Find first space, going backwards, in a string. */
int _findFirstSpace(char *s, int starting_pos, int ending_pos)
{
  int line_length = strlen(s);

  if (starting_pos == -1 || starting_pos >= line_length)
    starting_pos = line_length - 1;

  if (ending_pos > starting_pos)
    return -1;

  if (ending_pos < 0)
    ending_pos = 0;

  for (int i = starting_pos - 1; i >= ending_pos; i--)
  {
    if (s[i] == ' ')
      return i;
  }

  return -1;
}

/* Private function. Calculates the width of a window. */
int _windowCalculateLongestLine(Window *w)
{
  int longest = 0;

  for (int i = 0; i < w->buffer_size; i++)
  {
    if (strlen(w->lines_buffer[i]) > longest)
    {
      longest = strlen(w->lines_buffer[i]);
    }
  }

  return longest;
}

/* Private function. Auto resize a window width. */
void _windowAutoWidth(Window *w, int longest)
{
  w->size.width = longest + 2 + w->padding * 2;
  return;
}

/* Private function. Auto resize a window height. */
void _windowAutoHeight(Window *w)
{
  w->size.height = w->buffer_size + 2;
  return;
}

/* Private function. Draw window border. */
void _windowDrawBorder(Window *w)
{
  const int width = w->size.width;
  const int height = w->size.height;

  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
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
    return (w->size.width - w->padding * 2 - 2 - current_len) / 2;

  else if (w->alignment == 1)
    return w->size.width - w->padding * 2 - 2 - current_len;

  return 0;
}

/* Private function. Wrap lines. */
int _windowLinesWrap(Window *w)
{
  int current = 0;
  const int width = w->size.width - 2 * w->padding;
  // go over each line
  for (int i = 0; i < w->buffer_size; i++)
  {
    const int len = strlen(w->lines_buffer[i]);
    // if the line is too long
    if (len > width)
    {
      // break it into smaller parts
      int current_pos = 0;
      while (current_pos < len)
      {
        int first_space, copy_size;
        // check that the chunk is in bound of the string
        first_space = _findFirstSpace(w->lines_buffer[i], current_pos + width, current_pos);
        copy_size = first_space - current_pos;

        if (copy_size <= 0)
          copy_size = len;

        if (copy_size + current_pos > len)
          copy_size = len - current_pos;

        // copy to display lines
        strncpy(w->display_lines[current], w->lines_buffer[i] + current_pos, copy_size);

        current++;
        current_pos += copy_size;
      }
    }
    else
    {
      // simply copy into display line
      strcpy(w->display_lines[current], w->lines_buffer[i]);
      current++;
    }
  }

  // check if the window has to be resized or some lines have to be hidden
  if (current > w->size.height - 2 && w->auto_height)
    w->size.height = current + 2;
  else if (w->size.height < current - 2)
    current = w->size.height - 2;

  w->buffer_size = current;

  // copy back into display lines to prevent double wrapping
  for (int i = 0; i < w->buffer_size; i++)
    strcpy(w->lines_buffer[i], w->display_lines[i]);

  return current;
}

/* Private function. Copy lines from buffer to display */
int _windowLinesUnbuffer(Window *w)
{
  for (int i = 0; i < w->buffer_size; i++)
    strcpy(w->display_lines[i], w->lines_buffer[i]);

  return w->buffer_size;
}

/* Private function. Clears a window buffer. */
void _windowClearUnbuffered(Window *w)
{
  w->buffer_size = 0;
}

/* Private function. Pad string with spaces. */
void _padString(char *dest, char *source, int chars)
{
  char buffer[100];
  const int len = strlen(source);

  for (int i = 0; i < chars; i++)
    buffer[i] = ' ';

  for (int i = 0; i < len; i++)
    buffer[i + chars] = source[i];

  for (int i = chars + len; i < 2 * chars + len; i++)
    buffer[i] = ' ';

  buffer[strlen(source) + 2 * chars] = '\0';
  strcpy(dest, buffer);

  // for (int i = 0; i < chars; i++)
  //   dest[strlen(source) + chars + i] = ' ';

  // dest[strlen(dest)] = '\0';
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
    return (Rectangle){.width = -1, .height = -1};

  return (Rectangle){.width = w, .height = h};
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
  // allocate space for window
  Window *w = malloc(sizeof(Window));
  // pack struct
  *w = (Window){
      .auto_width = 1,
      .auto_height = 1,
      .buffer_size = 0,
      .padding = 1,
      .alignment = -1,
      .line_wrap = 1,
      .visible = 1,
      .fg_color = fg_DEFAULT,
      .bg_color = bg_DEFAULT,
      .text_style = text_DEFAUlT,
      .size = size,
      .pos = pos,
  };

  return w;
}

/* Deletes a Window. */
void deleteWindow(Window *w)
{
  if (w)
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
  tcsetattr(0, TCSANOW, &term);

  printf(HIDECURSOR);
  fflush(NULL);

  return;
}

/* Makes echo and terminals visible again. */
void show_cursor()
{
  // turn on echo
  struct termios term;
  tcgetattr(0, &term);
  term.c_lflag |= ECHO;
  tcsetattr(0, TCSANOW, &term);

  printf(SHOWCURSOR);
  fflush(NULL);

  return;
}

/* Enters terminal raw mode. */
void enter_raw_mode()
{
  struct termios raw;
  tcgetattr(0, &raw);
  raw.c_lflag &= ~(ICANON | ECHO);
  raw.c_cc[VTIME] = 0;
  raw.c_cc[VMIN] = 0;
  tcsetattr(0, TCSANOW, &raw);
  fflush(NULL);
}

/* Leaves terminal raw mode. */
void exit_raw_mode()
{
  struct termios raw;
  tcgetattr(0, &raw);
  raw.c_lflag |= (ICANON | ECHO);
  raw.c_cc[VTIME] = 0;
  raw.c_cc[VMIN] = 1;
  tcsetattr(0, TCSANOW, &raw);
  fflush(NULL);
}

/* Moves the cursor to the bottom of the screen. */
void move_cursor_to_bottom()
{
  Rectangle terminal_size = get_terminal_size();
  if (terminal_size.width != -1 && terminal_size.height != -1)
    move_cursor_to(0, terminal_size.height);

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
  printf(ESCAPE "[%i;%iH", y, x + 1);
  fflush(NULL);

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
  set_textmode(text_DEFAUlT);
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

/* Polls a keypress. Returns the code corresponding to the key. */
char poll_keypress()
{
  char buf;
  enter_raw_mode();

  if (read(0, &buf, 1) == 0)
    buf = 0;

  exit_raw_mode();

  return buf;
}

/* Polls special key presses. Return value: the 8 lsb represent (from left to right):
BACKSPACE SPACEBAR ENTER TAB RIGHT LEFT DOWN UP */
char poll_special_keypress()
{
  char key;
  char pressed, status;
  // to catch arrrow keypressed we need to read 3 characters
  // 2 delimiters and 1 for the actual button
  // implemented as a simple FSM
  // the 8 lsb represent (from left to right)
  // BACKSPACE SPACEBAR ENTER TAB RIGHT LEFT DOWN UP pressed.
  pressed = 0;
  status = 0;
  while ((key = poll_keypress()) > 0)
  {
    if (key == 27)
    {
      // first step of the FSM
      // first escape character found
      status |= 0b00000001;
    }
    else if (key == 91 && status & 0b00000001)
    {
      // second step of the FSM
      // second escape character found
      status |= 0b00000010;
    }
    else if (key >= 65 && key <= 68 && status & 0b00000010)
    {
      // last step of the FSM
      // key character found
      switch (key)
      {
      case 65:
        pressed |= 0b00000001; // up
        break;
      case 66:
        pressed |= 0b00000010; // down
        break;
      case 67:
        pressed |= 0b00000100; // left
        break;
      case 68:
        pressed |= 0b00001000; // right
        break;

      default:
        break;
      }

      pressed &= 0b00001111;
      status &= 0b11110000;
    }
    else
    {
      // some keys are not escaped
      status = 0;
      switch (key)
      {
      case 9:
        pressed |= 0b00010000; // tab
        break;
      case 10:
        pressed |= 0b00100000; // enter
        break;
      case 32:
        pressed |= 0b01000000; // space bar
        break;
      case 127:
        pressed |= 0b10000000; // backspace
        break;
      default:
        break;
      }
    }

    // if either key was found, return
    if (pressed)
      break;
  }

  // hide cursor again
  hide_cursor();

  return pressed;
}

/* Awaits a keypress. A message is prompted on the terminal. Pass NULL to skip. */
int await_keypress(char *s)
{
  if (s != NULL)
    printf("%s", s);

  int read_bytes;
  char *buffer[1];

  enter_raw_mode();
  do
  {
    read_bytes = read(0, buffer, 1);
  } while (read_bytes == 0);
  exit_raw_mode();

  return read_bytes;
}

/* Awaits a enter keypress. A message is prompted on the terminal. Pass NULL to skip. */
int await_enter(char *s)
{
  if (s != NULL)
    printf("%s", s);

  return getchar();
}

/* Sets size of a window. Size is relative to the outer border. */
void windowSetSize(Window *w, int width, int height)
{
  if (width < 0 || height < 0)
    return;

  if (width != w->size.width || height != w->size.height)
    w->size = createRectangle(width, height);

  return;
}

/* Sets width of a window. Size is relative to the outer border. */
void windowSetWidth(Window *w, int width)
{
  if (width < 0)
    return;

  if (width != w->size.width)
  {
    windowClear(w);
    w->size.width = width;
  }

  return;
}

/* Sets height of a window. Size is relative to the outer border. */
void windowSetHeight(Window *w, int height)
{
  if (height < 0)
    return;

  if (height != w->size.height)
  {
    windowClear(w);
    w->size.height = height;
  }

  return;
}

/* Sets visibility of a window. */
void windowSetVisibility(Window *w, int visibility)
{
  if (visibility != 0 && visibility != 1)
    return;

  w->visible = visibility;
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
      .x = w->pos.x + w->size.width,
      .y = w->pos.y + w->size.height,
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
/* Sets window auto size. Values: 1 for automatic sizing, 0 for manual sizing. */
void windowSetAutoSize(Window *w, int auto_size)
{
  if (auto_size != 0 && auto_size != 1)
    return;

  w->auto_height = auto_size;
  w->auto_width = auto_size;
}

/* Sets window auto width. Values: 1 for automatic width, 0 for manual sizing. */
void windowSetAutoWidth(Window *w, int auto_size)
{
  if (auto_size != 0 && auto_size != 1)
    return;

  w->auto_width = auto_size;
}

/* Sets window auto height. Values: 1 for automatic height, 0 for manual sizing. */
void windowSetAutoHeight(Window *w, int auto_size)
{
  if (auto_size != 0 && auto_size != 1)
    return;

  w->auto_height = auto_size;
}

/* Sets window line wrap. Values: 1 for automatic wrapping, 0 for no wrapping. */
void windowSetLineWrap(Window *w, int line_wrap)
{
  if (line_wrap != 0 && line_wrap != 1)
    return;

  w->line_wrap = line_wrap;
}

/* Manually trigger window resize. */
void windowAutoResize(Window *w)
{
  if (w->auto_width)
  {
    int longest;
    longest = _windowCalculateLongestLine(w);
    _windowAutoWidth(w, longest);
  }

  if (w->auto_height)
  {
    _windowAutoHeight(w);
  }
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

/* Sets window text style */
void windowSetTextStyle(Window *w, style textstyle)
{
  w->text_style = textstyle;
  return;
}

/* Returns the number of line of text of a window. */
int windowGetLines(Window *w)
{
  return w->buffer_size;
}

/* Adds a line of text to the window. Returns -1 in case of error, otherwise returns the size of the line. */
int windowAddLine(Window *w, char *line)
{
  if (w->buffer_size > MAX_LINES)
    return -1;

  strcpy(w->lines_buffer[w->buffer_size], line);
  w->buffer_size++;

  return sizeof(w->lines_buffer[w->buffer_size - 1]);
}

/* Changes a line of text in the window. Returns -1 in case of error, otherwise returns the size of the line. */
int windowChangeLine(Window *w, char *line, int line_count)
{
  if (line_count > w->buffer_size)
    return -1;

  strcpy(w->lines_buffer[line_count], line);

  return sizeof(w->lines_buffer[line_count]);
}

/* Deletes a line of text in the window. Returns -1 in case of error, otherwise returns the number of lines. */
int windowDeleteLine(Window *w, int line_count)
{
  if (line_count >= w->buffer_size)
    return -1;

  w->buffer_size--;

  for (int i = line_count; i < w->buffer_size; i++)
    strcpy(w->lines_buffer[i], w->lines_buffer[i + 1]);

  return w->buffer_size;
}

/* Deletes a all the lines of text in the window. Returns -1 in case of error. */
int windowDeleteAllLines(Window *w)
{
  for (int i = 0; i < w->buffer_size; i++)
    strcpy(w->lines_buffer[i], "");

  w->buffer_size = 0;

  return 0;
}

/* Shows a window on the terminal. */
void windowShow(Window *w)
{
  // hidden, return
  if (!w->visible)
    return;

  // calculate longest line
  int longest = _windowCalculateLongestLine(w);

  // auto resize window
  if (w->auto_width)
    _windowAutoWidth(w, longest);
  if (w->auto_height)
    _windowAutoHeight(w);

  // check if lines need to be wrapped
  const int width = w->size.width - 2 * w->padding;
  if (longest >= width && w->line_wrap)
    _windowLinesWrap(w); // windows are auto resized if needed
  else
    _windowLinesUnbuffer(w);

  // set styles
  set_fg(w->fg_color);
  set_bg(w->bg_color);

  // draw outer border
  _windowDrawBorder(w);
  fflush(NULL);

  // set text style
  // prevent complete reset
  if (w->text_style != text_DEFAUlT)
    set_textmode(w->text_style);

  for (int i = 0; i < w->buffer_size; i++)
  {
    // calculate line length
    const int ll = strlen(w->display_lines[i]);
    // calculate spacing according to alignment
    int spacing = _windowCalculateSpacing(w, ll);
    // calculate line coordinates
    const int lx = w->pos.x + w->padding + spacing + 1;
    const int ly = w->pos.y + i + 1;
    // draw text
    move_cursor_to(lx, ly);
    printf("%s", w->display_lines[i]);
    fflush(NULL);
  }

  reset_styles();
  move_cursor_to(0, 0);
}

/* Clears a window from the terminal */
void windowClear(Window *w)
{
  reset_bg();
  for (int y = 0; y < w->size.height; y++)
    erase_at(w->pos.x, y + w->pos.y, w->size.width);

  // clear window buffer
  _windowClearUnbuffered(w);
}

/* Creates a dialog window. */
Dialog *createDialog(int x, int y)
{
  int width, height;
  // get terminal size
  Rectangle r = get_terminal_size();
  // calculate size
  width = _min(DIALOG_MAX_WIDTH, r.width);
  height = _min(DIALOG_MAX_HEIGHT, r.height);
  // calculate border
  // create a window
  Window *w = createWindow(x, y);
  // manual size it
  windowSetSize(w, width, height);
  windowSetAutoSize(w, 0);

  // create buttons
  Window *b1, *b2;
  b1 = createWindow(x + 4, y - 4 + height);
  b2 = createWindow(x - 11 - 4 + width, y - 4 + height);
  // set alignment
  windowSetAlignment(b1, 0);
  windowSetAlignment(b2, 0);
  windowSetAlignment(w, 0);
  // set padding
  windowSetPadding(b1, 0);
  windowSetPadding(b2, 0);

  // allocate space for dialog
  Dialog *d = malloc(sizeof(Window));
  // pack struct
  *d = (Dialog){
      .window = w,
      .buttons = {b1, b2},
      .active_button = 0,
  };

  return d;
}

/* Deletes a dialog window. */
void deleteDialog(Dialog *d)
{
  // free window
  deleteWindow(d->window);
  // free buttons
  for (int i = 0; i < 2; i++)
    deleteWindow(d->buttons[i]);
  free(d);
}

/* Show a dialog */
void dialogShow(Dialog *d)
{
  windowShow(d->window);

  for (int i = 0; i < 2; i++)
  {
    windowSetTextStyle(d->buttons[i], i == d->active_button ? text_REVERSE : text_DEFAUlT);
    windowShow(d->buttons[i]);
  }
}

/* Hides a dialog */
void dialogClear(Dialog *d)
{
  windowClear(d->window);

  for (int i = 0; i < 2; i++)
    windowClear(d->buttons[i]);
}

/* Set yes/no buttons for the dialog. */
void dialogSetButtons(Dialog *d, char *yes, char *no)
{
  char buffer[100];

  windowDeleteAllLines(d->buttons[0]);
  windowDeleteAllLines(d->buttons[1]);

  _padString(buffer, yes, 2);
  windowAddLine(d->buttons[1], buffer);

  _padString(buffer, no, 2);
  windowAddLine(d->buttons[0], buffer);
}

/* Sets dialog padding. */
void dialogSetPadding(Dialog *d, int padding)
{
  if (padding > 0)
    d->window->padding = padding;

  return;
}

/* Sets dialog text. */
void dialogSetText(Dialog *d, char *text, int v_padding)
{
  windowDeleteAllLines(d->window);

  for (int i = 0; i < v_padding; i++)
    windowAddLine(d->window, "");

  windowAddLine(d->window, text);
  return;
}

int dialogWaitResponse(Dialog *d)
{
  while (1)
  {

    int special = poll_special_keypress();

    if (special & 0b00000100) // left
      d->active_button = 1;
    else if (special & 0b00001000) // right
      d->active_button = 0;
    else if (special & 0b00010000) // tab
      d->active_button = !d->active_button;
    else if (special & 0b00100000) // enter
      return d->active_button;

    if (special)
      dialogShow(d);
  }
}