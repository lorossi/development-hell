#include "terminal.h"

int main()
{
  //clear_terminal();
  HSL c1 = createHSLcolor(303, 72, 50);
  RGB c2 = HSLtoRGB(c1);
  printf("%i %i %i\n", c2.R, c2.G, c2.B);
  c2 = createRGBcolor(42, 71, 26);
  c1 = RGBtoHSL(c2);
  printf("%i %i %i\n", c1.H, c1.S, c1.L);
  return 0;
}