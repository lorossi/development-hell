#ifndef _TERMINAL
#define _TERMINAL

#include "colors.h"

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

void clear_screen();
void hide_cursor();
void show_cursor();
void move_to(Position p);
Rectangle get_size();
void write_at(Position p, char *s);
void write_at_RGB(Position p, RGB color, char *s);

#endif