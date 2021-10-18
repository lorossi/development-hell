#ifndef _TERMINAL
#define _TERMINAL

#include "terminal.h"
#include "colors.h"

#include <stdio.h>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h>    // for STDOUT_FILENO
#include <math.h>      // for sin, cos
#include <termios.h>

#define ESCAPE "\x1b"
#define CLEARALL ESCAPE "[2J"
#define RESET ESCAPE "[0M"
#define MOVEHOME ESCAPE "[H"
#define HIDECURSOR ESCAPE "[?25l"
#define SHOWCURSOR ESCAPE "[?25h"

typedef struct
{
  int w, h;
} Rectangle;

typedef struct
{
  int x, y;
} Position;

typedef struct termios Terminal;

Terminal set_raw();
void unset_raw(Terminal old);
void clear_screen();
void hide_cursor();
void show_cursor();
void move_to(Position p);
Rectangle get_size();
void write_at(Position p, char *s);
void write_int_at(Position p, int i);
void write_at_RGB(Position p, RGB color, char *s);

#endif