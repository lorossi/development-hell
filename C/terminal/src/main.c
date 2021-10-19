#include "terminal.h"
#include <time.h>   // for random init
#include <stdlib.h> // for random values
#include <stdio.h>  // for I/O operations

void test_colors()
{
  reset_styles();
  clear_terminal();

  /* test foreground colors */
  move_cursor_to(0, 0);
  set_fg(fg_BLACK);
  printf("Color: black");

  move_cursor_to(0, 1);
  set_fg(fg_RED);
  printf("Color: red");

  move_cursor_to(0, 2);
  set_fg(fg_GREEN);
  printf("Color: green");

  move_cursor_to(0, 3);
  set_fg(fg_YELLOW);
  printf("Color: yellow");

  move_cursor_to(0, 4);
  set_fg(fg_BLUE);
  printf("Color: blue");

  move_cursor_to(0, 5);
  set_fg(fg_MAGENTA);
  printf("Color: magenta");

  move_cursor_to(0, 6);
  set_fg(fg_CYAN);
  printf("Color: cyan");

  move_cursor_to(0, 7);
  set_fg(fg_WHITE);
  printf("Color: white");

  move_cursor_to(0, 8);
  set_fg(fg_DEFAULT);
  printf("Color: default");

  move_cursor_to(15, 0);
  set_fg(fg_BRIGHT_BLACK);
  printf("Color: bright black");

  move_cursor_to(15, 1);
  set_fg(fg_BRIGHT_RED);
  printf("Color: bright red");

  move_cursor_to(15, 2);
  set_fg(fg_BRIGHT_GREEN);
  printf("Color: bright green");

  move_cursor_to(15, 3);
  set_fg(fg_BRIGHT_YELLOW);
  printf("Color: bright yellow");

  move_cursor_to(15, 4);
  set_fg(fg_BRIGHT_BLUE);
  printf("Color: bright blue");

  move_cursor_to(15, 5);
  set_fg(fg_BRIGHT_MAGENTA);
  printf("Color: bright magenta");

  move_cursor_to(15, 6);
  set_fg(fg_BRIGHT_CYAN);
  printf("Color: bright cyan");

  move_cursor_to(15, 7);
  set_fg(fg_BRIGHT_WHITE);
  printf("Color: bright white");

  reset_fg();
  /* test background colors */
  move_cursor_to(0, 9);
  set_bg(bg_BLACK);
  printf("Color: black");

  move_cursor_to(0, 10);
  set_bg(bg_RED);
  printf("Color: red");

  move_cursor_to(0, 11);
  set_bg(bg_GREEN);
  printf("Color: green");

  move_cursor_to(0, 12);
  set_bg(bg_YELLOW);
  printf("Color: yellow");

  move_cursor_to(0, 13);
  set_bg(bg_BLUE);
  printf("Color: blue");

  move_cursor_to(0, 14);
  set_bg(bg_MAGENTA);
  printf("Color: magenta");

  move_cursor_to(0, 15);
  set_bg(bg_CYAN);
  printf("Color: cyan");

  move_cursor_to(0, 16);
  set_bg(bg_WHITE);
  printf("Color: white");

  move_cursor_to(0, 17);
  set_bg(bg_DEFAULT);
  printf("Color: default");

  move_cursor_to(15, 9);
  set_bg(bg_BRIGHT_BLACK);
  printf("Color: bright black");

  move_cursor_to(15, 10);
  set_bg(bg_BRIGHT_RED);
  printf("Color: bright red");

  move_cursor_to(15, 11);
  set_bg(bg_BRIGHT_GREEN);
  printf("Color: bright green");

  move_cursor_to(15, 12);
  set_bg(bg_BRIGHT_YELLOW);
  printf("Color: bright yellow");

  move_cursor_to(15, 13);
  set_bg(bg_BRIGHT_BLUE);
  printf("Color: bright blue");

  move_cursor_to(15, 14);
  set_bg(bg_BRIGHT_MAGENTA);
  printf("Color: bright magenta");

  move_cursor_to(15, 15);
  set_bg(bg_BRIGHT_CYAN);
  printf("Color: bright cyan");

  move_cursor_to(15, 16);
  set_bg(bg_BRIGHT_WHITE);
  printf("Color: bright white");

  printf("\n");

  reset_styles();
  return;
}

void test_textmodes()
{
  reset_styles();
  clear_terminal();

  set_textmode(BOLD);
  printf("BOLD TEXT\n");
  reset_textmode();

  set_textmode(FAINT);
  printf("FAINT TEXT\n");
  reset_textmode();

  set_textmode(ITALIC);
  printf("ITALIC TEXT\n");
  reset_textmode();

  set_textmode(UNDERLINE);
  printf("UNDERLINE TEXT\n");
  reset_textmode();

  set_textmode(BLINKING);
  printf("BLINKING TEXT\n");
  reset_textmode();

  set_textmode(REVERSE);
  printf("REVERSE TEXT\n");
  reset_textmode();

  set_textmode(HIDDEN);
  printf("HIDDEN TEXT");
  reset_textmode();
  printf(" (hidden) \n");

  set_textmode(STRIKETHROUGH);
  printf("STRIKETHROUGH TEXT\n");
  reset_textmode();

  reset_styles();
  return;
}

void test_mixed()
{
  reset_styles();
  clear_terminal();
  move_cursor_to(0, 0);

  reset_styles();
  set_styles(3, BOLD, fg_RED, bg_BRIGHT_WHITE);
  printf("Italic, red on bright white\n");

  reset_styles();
  set_styles(4, FAINT, ITALIC, fg_YELLOW, bg_BRIGHT_CYAN);
  printf("Faint italic, yellow on bright cyan\n");

  reset_styles();
  set_styles(4, fg_BRIGHT_YELLOW, bg_BRIGHT_GREEN, BLINKING, UNDERLINE);
  printf("Blinking underlined, bright yellow on bright green\n");

  reset_styles();
}

void test_RGB()
{
  const int inc = 2;

  reset_styles();
  clear_terminal();
  move_cursor_to(0, 0);

  set_fg(fg_RED);
  printf("RED:   ");

  for (int r = 255; r >= 0; r -= inc)
  {
    RGB color = createRGBcolor(r, 0, 0);
    set_fg_RGB(color);
    printf("@");
  }

  printf("\n");

  set_fg(fg_GREEN);
  printf("GREEN: ");

  for (int g = 255; g >= 0; g -= inc)
  {
    RGB color = createRGBcolor(0, g, 0);
    set_fg_RGB(color);
    printf("@");
  }

  printf("\n");

  set_fg(fg_BLUE);
  printf("BLUE:  ");

  for (int b = 255; b >= 0; b -= inc)
  {
    RGB color = createRGBcolor(0, 0, b);
    set_fg_RGB(color);
    printf("@");
  }

  printf("\n\n");
  reset_styles();

  reset_bg();
  set_fg(fg_RED);
  printf("RED:   ");

  for (int r = 255; r >= 0; r -= inc)
  {
    RGB color = createRGBcolor(r, 0, 0);
    set_bg_RGB(color);
    printf(" ");
  }

  printf("\n");

  reset_bg();
  set_fg(fg_GREEN);
  printf("GREEN: ");

  for (int g = 255; g >= 0; g -= inc)
  {
    RGB color = createRGBcolor(0, g, 0);
    set_bg_RGB(color);
    printf(" ");
  }

  printf("\n");

  reset_bg();
  set_fg(fg_BLUE);
  printf("BLUE:  ");

  for (int b = 255; b >= 0; b -= inc)
  {
    RGB color = createRGBcolor(0, 0, b);
    set_bg_RGB(color);
    printf(" ");
  }

  printf("\n");

  reset_styles();
}

void test_HSL()
{
  clear_terminal();
  move_cursor_to(0, 0);
  reset_styles();

  const int inc = 6;
  int x, y;

  x = 0;
  y = 0;
  for (int h = 0; h < 360; h += inc)
  {
    for (int s = 100; s >= 0; s -= inc)
    {
      HSL color = createHSLcolor(h, s, 50);

      move_cursor_to(x, y);
      set_fg_HSL(color);
      printf("@");

      y++;
    }

    x++;
    y = 0;
  }

  reset_styles();

  x = 0;
  y = 0;
  for (int h = 0; h < 360; h += inc)
  {
    for (int s = 100; s >= 0; s -= inc)
    {
      HSL color = createHSLcolor(h, s, 50);

      move_cursor_to(x, y + 100 / inc + 2);
      set_bg_HSL(color);
      printf(" ");

      y++;
    }

    x++;
    y = 0;
  }

  printf("\n\n");
  reset_styles();
}

void test_hue()
{
  clear_terminal();
  move_cursor_to(0, 0);
  reset_styles();

  const int inc = 3;
  int x = 0;

  for (int h = 0; h < 360; h += inc)
  {
    RGB color = HUEtoRGB(h);
    reset_bg();
    move_cursor_to(x, 0);
    set_fg_RGB(color);
    printf("@");

    move_cursor_to(x, 1);
    set_bg_RGB(color);
    printf(" ");
    x++;
  }

  reset_styles();
}

void starry_night()
{
  RGB sky_color = createRGBcolor(25, 25, 112);
  RGB star_color = createRGBcolor(255, 205, 60);
  Rectangle size = get_terminal_size();

  reset_styles();
  set_bg_RGB(sky_color);
  set_fg_RGB(star_color);
  set_textmode(BOLD);
  set_textmode(BLINKING);

  clear_terminal();

  srand(time(NULL));
  for (int i = 0; i < 25; i++)
  {
    int x = rand() % size.w;
    int y = rand() % size.h;
    move_cursor_to(x, y);
    printf("*");
  }

  move_cursor_to_bottom();
  reset_styles();
}

int main()
{
  test_colors();
  printf("Press enter to continue");
  getchar();

  test_textmodes();
  printf("Press enter to continue");
  getchar();

  test_mixed();
  printf("Press enter to continue\n");
  getchar();

  test_RGB();
  printf("Press enter to continue\n");
  getchar();

  test_HSL();
  printf("Press enter to continue\n");
  getchar();

  test_hue();
  printf("Press enter to continue\n");
  getchar();

  starry_night();
  printf("Press enter to continue\n");
  getchar();

  clear_terminal();
  return 0;
}