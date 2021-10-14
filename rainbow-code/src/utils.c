#include <math.h>
#include "utils.h"

double _wrap(double x, double max)
{
  while (x > max)
    x -= max;
  while (x < 0)
    x += max;

  return x;
}

double _min(double a, double b)
{
  if (a < b)
    return a;
  return b;
}

double _max(double a, double b)
{
  if (a > b)
    return a;
  return b;
}

double _min3(double a, double b, double c)
{
  return _min(_min(a, b), c);
}

RGB posToRGB(RelativePosition rpos, RelativePosition center, double time)
{
  double d;
  d = sqrt(pow(rpos.x - center.x, 2.0) + pow(rpos.y - center.y, 2.0)) - time;
  double hue;
  hue = _wrap(d / 3, 1.0);

  return HUEtoRGB(hue);
}

RGB HUEtoRGB(double hue)
{
  double kr, kg, kb;
  kr = fmod(5.0 + hue * 6.0, 6.0);
  kg = fmod(3.0 + hue * 6.0, 6.0);
  kb = fmod(1.0 + hue * 6.0, 6.0);

  kr = 1.0 - _max(_min3(kr, 4.0 - kr, 1.0), 0.0);
  kg = 1.0 - _max(_min3(kg, 4.0 - kg, 1.0), 0.0);
  kb = 1.0 - _max(_min3(kb, 4.0 - kb, 1.0), 0.0);

  return (RGB){kr * 255, kg * 255, kb * 255};
}
