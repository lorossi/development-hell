#ifndef _TERMINAL
#define _TERMINAL

#include "utils.h"

#define ESCAPE "\x1b"
#define CLEARALL ESCAPE "[2J"
#define RESET ESCAPE "[0M"
#define MOVEHOME ESCAPE "[H"
#define HIDECURSOR ESCAPE "[?25l"
#define SHOWCURSOR ESCAPE "[?25h"

void clear_screen();
void hide_cursor();
void show_cursor();
Rectangle get_size();
void write_at(Position p, char c);
void write_at_colored(Position p, RelativePosition rp, RelativePosition center, double time, char c);
void write_at_colored_block(Position p, RelativePosition rp, RelativePosition center, double time);
void write_at_float(Position p, float f);
void move_to(Position p);
RelativePosition time_position(double time);

#endif