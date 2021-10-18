#ifndef _UTILS
#define _UTILS

typedef struct
{
  int r;
  int g;
  int b;
} RGB;

typedef struct
{
  int x;
  int y;
} Position;

typedef struct
{
  double x;
  double y;
} RelativePosition;

typedef struct
{
  int w;
  int h;
} Rectangle;

RGB posToRGB(RelativePosition rpos, RelativePosition center, double time);
RGB HUEtoRGB(double hue);

#endif