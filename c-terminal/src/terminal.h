#ifndef _TERMINAL
#define _TERMINAL

#include <stdio.h>
#include <sys/ioctl.h> // ioctl() and TIOCGWINSZ
#include <unistd.h>    // for STDOUT_FILENO
#include <termios.h>   // for cl_flag
#include <stdarg.h>    // for multiple parameters

#include "colors.h" // for.. well.. colors

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

// struct creation
Rectangle createRectangle(int w, int h);

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
void write_at(int x, int y, char *s);
void write_at_RGB(int x, int y, RGB color, char *s);
void erase_at(int x, int y, int length);

#endif