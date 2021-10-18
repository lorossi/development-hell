#include "colors.h"

double _hue_to_rgb(double p, double q, double t)
{
  if (t < 0)
    t += 1;
  if (t > 1)
    t -= 1;

  if (t < 1.0 / 6)
    return p + (q - p) * 6.0 * t;
  if (t < 1.0 / 2)
    return q;
  if (t < 2.0 / 3)
    return p + (q - p) * (2.0 / 3 - t) * 6.0;

  return p;
}

double _max(double a, double b)
{
  if (a > b)
    return a;
  return b;
}

double _min(double a, double b)
{
  if (a < b)
    return a;
  return b;
}

double _max_3(double a, double b, double c)
{
  return _max(_max(a, b), c);
}

double _min_3(double a, double b, double c)
{
  return _min(_min(a, b), c);
}

RGB createRGBcolor(int R, int G, int B)
{
  return (RGB){.R = R, .G = G, .B = B};
}

HSL createHSLcolor(int H, int S, int L)
{
  return (HSL){.H = H, .S = S, .L = L};
}

RGB HSLtoRGB(HSL color)
{
  double h, s, l;
  h = (double)color.H / 360;
  s = (double)color.S / 100;
  l = (double)color.L / 100;

  double r, g, b;
  if (s == 0)
  {
    r = l;
    g = l;
    b = l;
  }
  else
  {
    double q, p;
    q = l < 0.5 ? l * (1 + s) : l + s - l * s;
    p = 2 * l - q;

    r = _hue_to_rgb(p, q, h + 1.0 / 3);
    g = _hue_to_rgb(p, q, h);
    b = _hue_to_rgb(p, q, h - 1.0 / 3);
  }

  return createRGBcolor(r * 255, g * 255, b * 255);
}

HSL RGBtoHSL(RGB color)
{
  double r, g, b;
  r = (double)color.R / 255;
  g = (double)color.G / 255;
  b = (double)color.B / 255;

  double max, min;
  max = _max_3(r, g, b);
  min = _min_3(r, g, b);

  double h, s, l;
  l = (max + min) / 2;

  if (max == min)
  {
    h = 0;
    s = 0;
  }
  else
  {
    double d;
    d = max - min;
    s = l > 0.5 ? d / (2 - max - min) : d / (max + min);

    if (max == r)
      h = (g - b) / d + (g < b ? 6 : 0);
    else if (max == g)
      h = (b - r) / d + 2;
    else if (max == b)
      h = (r - g) / d + 4;
  }

  return createHSLcolor(h * 60, s * 100, l * 100);
}