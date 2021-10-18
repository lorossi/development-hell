#include "keys.h"

Key createKey(int code, char *label)
{
  return (Key){
      .code = code,
      .label = label,
      .pressed = 0,
      .last_event = 0,
  };
}

int key_get_code(Key *k)
{
  return k->code;
}

int key_get_pressed(Key *k)
{
  return k->pressed;
}

char *key_get_label(Key *k)
{
  return k->label;
}

void key_set_pressed(Key *k, int pressed)
{
  k->pressed = pressed;
  k->last_event = clock();
  return;
}

int key_get_msec_since_action(Key *k)
{
  Time now = clock();

  return (double)(now - k->last_event) / CLOCKS_PER_SEC * 1000;
}

int poll_keypress()
{
  char buffer[2];

  if (read(2, buffer, 2) != 0)
    return (int)buffer[0];

  return -1;
}