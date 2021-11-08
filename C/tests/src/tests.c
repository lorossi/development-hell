#include <stdio.h>
#include <wchar.h>
#include <locale.h>

int main()
{
  setlocale(LC_CTYPE, ""); // i don't know

  for (wchar_t i = 0x3041; i <= 0x3096; i++)
    wprintf(L"%lc\n", i); // i don't know

  return 0;
}