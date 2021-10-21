#ifndef _TERMINAL
#define _TERMINAL

#include <stdio.h>
#include <sys/ioctl.h> // ioctl() and TIOCGWINSZ
#include <unistd.h>    // for STDOUT_FILENO
#include <termios.h>   // for cl_flag
#include <stdarg.h>    // for multiple parameters
#include <string.h>    // for strcpy()
#include <stdlib.h>    // for malloc() and free()

typedef int style;

static const style fg_BLACK = 30;
static const style fg_RED = 31;
static const style fg_GREEN = 32;
static const style fg_YELLOW = 33;
static const style fg_BLUE = 34;
static const style fg_MAGENTA = 35;
static const style fg_CYAN = 36;
static const style fg_WHITE = 37;
static const style fg_DEFAULT = 39;

static const style fg_BRIGHT_BLACK = 90;
static const style fg_BRIGHT_RED = 91;
static const style fg_BRIGHT_GREEN = 92;
static const style fg_BRIGHT_YELLOW = 93;
static const style fg_BRIGHT_BLUE = 94;
static const style fg_BRIGHT_MAGENTA = 95;
static const style fg_BRIGHT_CYAN = 96;
static const style fg_BRIGHT_WHITE = 97;

static const style bg_BLACK = 40;
static const style bg_RED = 41;
static const style bg_GREEN = 42;
static const style bg_YELLOW = 43;
static const style bg_BLUE = 44;
static const style bg_MAGENTA = 45;
static const style bg_CYAN = 46;
static const style bg_WHITE = 47;
static const style bg_DEFAULT = 49;

static const style bg_BRIGHT_BLACK = 100;
static const style bg_BRIGHT_RED = 101;
static const style bg_BRIGHT_GREEN = 102;
static const style bg_BRIGHT_YELLOW = 103;
static const style bg_BRIGHT_BLUE = 104;
static const style bg_BRIGHT_MAGENTA = 105;
static const style bg_BRIGHT_CYAN = 106;
static const style bg_BRIGHT_WHITE = 107;

static const style text_BOLD = 1;
static const style text_FAINT = 2;
static const style text_ITALIC = 3;
static const style text_UNDERLINE = 4;
static const style text_BLINKING = 5;
static const style text_REVERSE = 7;
static const style text_HIDDEN = 8;
static const style text_STRIKETHROUGH = 9;
static const style text_DEFAUlT = 0;

#define ESCAPE "\x1b"
#define CLEARALL ESCAPE "[2J"
#define RESET ESCAPE "[0M"
#define MOVEHOME ESCAPE "[H"
#define HIDECURSOR ESCAPE "[?25l"
#define SHOWCURSOR ESCAPE "[?25h"
#define BELL "\a"

#define MAX_LINES 10
#define MAX_WIDTH 250

typedef struct rect
{
  int width, height;
} Rectangle;

typedef struct pos
{
  int x, y;
} Position;

typedef struct rgb
{
  int R; // range [0-255]
  int G; // range [0-255]
  int B; // range [0-255]
} RGB;

typedef struct hsl
{
  int H; // range [0-360]
  int S; // range [0-100]
  int L; // range [0-100]
} HSL;

typedef struct window
{
  int auto_width, auto_height;
  int buffer_size;
  int padding;
  int alignment;
  int line_wrap;
  int visible;
  style fg_color;
  style bg_color;
  style text_style;
  char lines_buffer[MAX_LINES][MAX_WIDTH];
  char display_lines[MAX_LINES][MAX_WIDTH];
  Rectangle size;
  Position pos;
} Window;

typedef struct dialog
{
  Window *window;
} Dialog;

// struct creation
Rectangle createRectangle(int w, int h);
Position createPosition(int x, int y);
RGB createRGBcolor(int R, int G, int B);
HSL createHSLcolor(int H, int S, int L);

// colors manipulation
RGB HSLtoRGB(HSL color);
RGB HUEtoRGB(double hue);
HSL RGBtoHSL(RGB color);

// terminal manipulation
Rectangle get_terminal_size();
void clear_terminal();
void hide_cursor();
void show_cursor();
void move_cursor_to_bottom();
void move_cursor_to(int x, int y);
void enter_raw_mode();
void exit_raw_mode();
void set_styles(style styles, ...);
void reset_styles();
void set_fg(style color);
void set_bg(style color);
void set_textmode(style mode);
void reset_fg();
void reset_bg();
void reset_textmode();
void set_fg_RGB(RGB color);
void set_bg_RGB(RGB color);
void set_fg_HSL(HSL color);
void set_bg_HSL(HSL color);
void write_at(int x, int y, char *s);
void erase_at(int x, int y, int length);
int poll_keypress(char *buffer);
int await_keypress(char *s);
int await_enter(char *s);

// window manipulation
Window *createWindow(int x, int y);
void deleteWindow(Window *w);
void windowSetSize(Window *w, int width, int height);
void windowSetWidth(Window *w, int width);
void windowSetHeight(Window *w, int height);
void windowSetVisibility(Window *w, int visibility);
Rectangle windowGetSize(Window *w);
void windowSetPosition(Window *w, int x, int y);
Position windowGetPosition(Window *w);
Position windowGetBottomRight(Window *w);
void windowSetPadding(Window *w, int padding);
void windowSetAlignment(Window *w, int alignment);
void windowSetAutoWidth(Window *w, int auto_width);
void windowSetAutoHeight(Window *w, int auto_width);
void windowSetLineWrap(Window *w, int line_wrap);
void windowAutoResize(Window *w);
void windowSetFGcolor(Window *w, style fg_color);
void windowSetBGcolor(Window *w, style bg_color);
void windowSetTextStyle(Window *w, style textstyle);
int windowGetLines(Window *w);
int windowAddLine(Window *w, char *line);
int windowChangeLine(Window *w, char *line, int line_count);
int windowDeleteLine(Window *w, int line_count);
int windowDeleteAllLines(Window *w);
void windowShow(Window *w);
void windowClear(Window *w);

// dialog manipulation
Dialog *createDialog(int border_x, int border_y);
void deleteDialog(Dialog *d);
#endif