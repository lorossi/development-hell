#ifndef _TERMINAL
#define _TERMINAL

#include <stdio.h>
#include <sys/ioctl.h> // ioctl() and TIOCGWINSZ
#include <unistd.h>    // for STDOUT_FILENO
#include <termios.h>   // for cl_flag
#include <stdarg.h>    // for multiple parameters

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

static const style BOLD = 1;
static const style FAINT = 2;
static const style ITALIC = 3;
static const style UNDERLINE = 4;
static const style BLINKING = 5;
static const style REVERSE = 6;
static const style HIDDEN = 7;
static const style STRIKETHROUGH = 8;
static const style TEXT_RESET = 9;

#define ESCAPE "\x1b"
#define CLEARALL ESCAPE "[2J"
#define RESET ESCAPE "[0M"
#define MOVEHOME ESCAPE "[H"
#define HIDECURSOR ESCAPE "[?25l"
#define SHOWCURSOR ESCAPE "[?25h"
#define BELL "\a"

typedef struct
{
  int w, h;
} Rectangle;

typedef struct
{
  int R; // range [0-255]
  int G; // range [0-255]
  int B; // range [0-255]
} RGB;

typedef struct
{
  int H; // range [0-360]
  int S; // range [0-100]
  int L; // range [0-100]
} HSL;

// struct creation
Rectangle createRectangle(int w, int h);
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
void move_cursor_to(int x, int y);
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

#endif