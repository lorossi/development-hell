#ifndef _CONSTANTS
#define CONSTANTS

#ifdef BLOCK
const int BORDER = 0;
const int XREPETITIONS = 4;
const int YREPETITIONS = 2;
const int SPACING = 1;
#else
const int BORDER = 1;
const int XREPETITIONS = 2;
const int YREPETITIONS = 2;
const int SPACING = 2;
#endif

const double PI = 3.14159265359;
const double TWO_PI = 2 * PI;
const double TIME_SCL = 0.25;

#endif