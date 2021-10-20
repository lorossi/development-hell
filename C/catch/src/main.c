

#include "terminal.h"
#include <signal.h> // for signal()
#include <ctype.h>  // for isprint()
#include <stdio.h>

volatile int loop;

void SIGINT_handler()
{
  // stop loop
  loop = 0;
  return;
}

int main()
{
  signal(SIGINT, SIGINT_handler);

  clear_terminal();
  hide_cursor();

  loop = 1;
  char buffer[4];
  int bytes;
  while (loop)
  {
    bytes = poll_keypress(buffer);

    if (bytes != 0)
    {
      for (int i = 0; i < bytes; i++)
      {
        if (isprint(buffer[i]))
          printf("%c", buffer[i]); // prints character
        else
          printf("%d", buffer[i]); // prints code value for character
      }
      printf(" ");
    }
  }

  show_cursor();
  clear_terminal();

  return 0;
}