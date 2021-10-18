#include "terminal.h"

int main()
{
  clear_terminal();
  set_styles(BOLD, bg_BRIGHT_BLUE, fg_BRIGHT_RED);
  printf("Hello, there!\n");
  return 0;
}