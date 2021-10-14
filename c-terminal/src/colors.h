#ifndef _COLORS
#define _COLORS

typedef int fg_color;
typedef int bg_color;
typedef int textmode;

static const fg_color fg_BLACK = 30;
static const fg_color fg_RED = 31;
static const fg_color fg_GREEN = 32;
static const fg_color fg_YELLOW = 33;
static const fg_color fg_BLUE = 34;
static const fg_color fg_MAGENTA = 35;
static const fg_color fg_CYAN = 36;
static const fg_color fg_WHITE = 37;
static const fg_color fg_DEFAULT = 39;

static const fg_color fg_BRIGHT_BLACK = 90;
static const fg_color fg_BRIGHT_RED = 91;
static const fg_color fg_BRIGHT_GREEN = 92;
static const fg_color fg_BRIGHT_YELLOW = 93;
static const fg_color fg_BRIGHT_BLUE = 94;
static const fg_color fg_BRIGHT_MAGENTA = 95;
static const fg_color fg_BRIGHT_CYAN = 96;
static const fg_color fg_BRIGHT_WHITE = 97;

static const bg_color bg_BLACK = 40;
static const bg_color bg_RED = 41;
static const bg_color bg_GREEN = 42;
static const bg_color bg_YELLOW = 43;
static const bg_color bg_BLUE = 44;
static const bg_color bg_MAGENTA = 45;
static const bg_color bg_CYAN = 46;
static const bg_color bg_WHITE = 47;
static const bg_color bg_DEFAULT = 49;

static const bg_color bg_BRIGHT_BLACK = 100;
static const bg_color bg_BRIGHT_RED = 101;
static const bg_color bg_BRIGHT_GREEN = 102;
static const bg_color bg_BRIGHT_YELLOW = 103;
static const bg_color bg_BRIGHT_BLUE = 104;
static const bg_color bg_BRIGHT_MAGENTA = 105;
static const bg_color bg_BRIGHT_CYAN = 106;
static const bg_color bg_BRIGHT_WHITE = 107;

static const textmode BOLD = 1;
static const textmode FAINT = 2;
static const textmode ITALIC = 3;
static const textmode UNDERLINE = 4;
static const textmode BLINKING = 5;
static const textmode REVERSE = 6;
static const textmode HIDDEN = 7;
static const textmode STRIKETHROUGH = 8;
static const textmode TEXTMODE_RESET = 9;

typedef struct
{
  int R, G, B;
} RGB;

typedef struct
{
  int H, S, L;
} HSL;

// struct creation
RGB createRGBcolor(int R, int G, int B);
HSL createHSLcolor(int H, int S, int L);

// colors manipulation
RGB HSLtoRGB(HSL color);
HSL RGBtoHSL(RGB color);

#endif