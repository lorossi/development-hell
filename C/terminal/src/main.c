#include "terminal.h"

void test_colors()
{
  reset_styles();
  clear_terminal();
  /* test foreground colors */
  for (int i = 0; i < 8; i++)
  {
    move_cursor_to(0, i);
    set_fg(30 + i);
    printf("Color #%i", i);
  }

  for (int i = 0; i < 8; i++)
  {
    move_cursor_to(10, i);
    set_fg(90 + i);
    printf("Color #%i (bright)", i);
  }

  reset_fg();
  /* test background colors */
  for (int i = 0; i < 8; i++)
  {
    move_cursor_to(30, i);
    set_bg(40 + i);
    printf("Color #%i", i);
  }

  for (int i = 0; i < 8; i++)
  {
    move_cursor_to(40, i);
    set_bg(100 + i);
    printf("Color #%i (bright)", i);
  }

  move_cursor_to(0, 8);
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
  printf("HIDDEN TEXT\n");
  reset_textmode();

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
  reset_styles();
  clear_terminal();
  move_cursor_to(0, 0);

  const int inc = 2;

  for (int r = 0; r < 255; r += inc)
  {
    RGB color = createRGBcolor(r, 0, 0);
    set_fg_RGB(color);
    printf("@");
  }

  printf("\n");
  for (int g = 0; g < 255; g += inc)
  {
    RGB color = createRGBcolor(0, g, 0);
    set_fg_RGB(color);
    printf("@");
  }

  printf("\n");

  for (int b = 0; b < 255; b += inc)
  {
    RGB color = createRGBcolor(0, 0, b);
    set_fg_RGB(color);
    printf("@");
  }

  printf("\n");

  reset_styles();

  for (int r = 0; r < 255; r += inc)
  {
    RGB color = createRGBcolor(r, 0, 0);
    set_bg_RGB(color);
    printf("-");
  }

  printf("\n");
  for (int g = 0; g < 255; g += inc)
  {
    RGB color = createRGBcolor(0, g, 0);
    set_bg_RGB(color);
    printf("-");
  }

  printf("\n");

  for (int b = 0; b < 255; b += inc)
  {
    RGB color = createRGBcolor(0, 0, b);
    set_bg_RGB(color);
    printf("-");
  }

  printf("\n");

  reset_styles();
}

void test_HSL()
{
}

void test_hue()
{
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

  clear_terminal();

  return 0;
}