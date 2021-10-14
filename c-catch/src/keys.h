#ifndef _KEYS
#define _KEYS

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define UP_ARROW 65
#define DOWN_ARROW 66
#define RIGHT_ARROW 67
#define LEFT_ARROW 68

#define W_KEY 119
#define D_KEY 100
#define S_KEY 115
#define A_KEY 97

typedef clock_t Time;

typedef struct
{
  int code;
  char pressed;
  char *label;
  Time last_event;
} Key;

Key createKey(int code, char *label);
int key_get_code(Key *k);
int key_get_pressed(Key *k);
char *key_get_label(Key *k);
void key_set_pressed(Key *k, int pressed);
int key_get_msec_since_action(Key *k);
int poll_keypress();

#endif