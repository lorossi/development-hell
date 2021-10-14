#ifndef _COLORS
#define _COLORS

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