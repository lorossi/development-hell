#include <pthread.h>
#include <signal.h> // for signal()
#include <stdio.h>
#include "terminal.h"
#include "keys.h"

// https: //www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
static volatile int loop = 1;
Key keys[4];

void SIGINT_handler()
{
  loop = 0;
  return;
}

void *getKeyThread()
{
  while (loop)
  {
    int keyCode = poll_keypress();

    // a key is pressed, check which one
    for (int i = 0; i < 4 && keyCode != -1; i++)
    {
      if (keyCode == key_get_code(&keys[i]))
        key_set_pressed(&keys[i], 1);
    }

    // no key has been pressed, reset all
    for (int i = 0; i < 4 && keyCode == -1; i++)
    {
      int msec = key_get_msec_since_action(&keys[i]);

      if (msec > 250)
        key_set_pressed(&keys[i], 0);
    }
  }
  return 0;
}

int main()
{
  signal(SIGINT, SIGINT_handler);

  keys[0] = createKey(W_KEY, "U");
  keys[1] = createKey(S_KEY, "D");
  keys[2] = createKey(A_KEY, "L");
  keys[3] = createKey(D_KEY, "R");

  pthread_t key_thread;
  pthread_create(&key_thread, NULL, &getKeyThread, NULL);
  Terminal old_term;
  old_term = set_raw();

  clear_screen();
  hide_cursor();

  while (loop)
  {

    {
      for (int i = 0; i < 4; i++)
      {
        Position pos;
        pos = (Position){.x = 0, .y = i};
        write_int_at(pos, key_get_pressed(&keys[i]));
        pos = (Position){.x = 3, .y = i};
        write_at(pos, key_get_label(&keys[i]));
      }
    }
  }

  unset_raw(old_term);
  show_cursor();
  clear_screen();

  return 0;
}