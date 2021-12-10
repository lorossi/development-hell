//#define DEBUG

#include <stdio.h> /* Standard Library of Input and Output */
#include <png.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <complex.h> /* Standard Library of Complex Numbers */
#include <math.h>    /* Standard Library of math */

const double X = 5;
const double Y = 5;
const double MINEXP = 1;
const double MAXEXP = 10;
const double COLORSPAN = 0.7;
const int COLORREPETITIONS = 1;
const char *FRAMESFOLDER = "output/frames/";

#ifdef DEBUG
const int ITEMS = 300;
const int FRAMES = 600;
const int WORKERS = 30;
const int MAXITERS = 5;
#else
const int ITEMS = 1000;
const int FRAMES = 900;
const int WORKERS = 25;
const int MAXITERS = 25;
#endif

#ifndef __cplusplus

double max(double a, double b)
{
  if (a > b)
    return a;
  return b;
}

double min(double a, double b)
{
  if (a < b)
    return a;
  return b;
}

#endif

/* A coloured pixel. */
typedef struct
{
  uint8_t R;
  uint8_t G;
  uint8_t B;
} Pixel;

/* A picture. */
typedef struct
{
  Pixel *pixels;
  size_t width;
  size_t height;
} Bitmap;

typedef struct
{
  uint32_t count;
  double exponent;
  double offset;
} Parameters;

double min3(double x, double y, double z)
{
  return min(min(x, y), z);
}

double easeOut(double x)
{
  return 1 - pow(1 - x, 2);
}

double easeInOut(double x)
{
  return x < 0.5 ? 2 * x * x : 1 - pow(-2 * x + 2, 2) / 2;
}

/* Get pixel corresponding to coordinates. 
Adapted from https://www.lemoda.net/c/write-png/ */
Pixel *pixel_at(Bitmap *bitmap, int x, int y)
{
  return bitmap->pixels + bitmap->width * y + x;
}

/* Write "bitmap" to a PNG file specified by "path"; returns 0 on
success, non-zero on error.
Adapted from https://www.lemoda.net/c/write-png/  */
int save_png_to_file(Bitmap *bitmap, const char *path)
{
  FILE *fp;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  size_t x, y;
  png_byte **row_pointers = NULL;

  int pixel_size = 3;
  int depth = 8;

  fp = fopen(path, "wb");
  if (!fp)
    return -1;

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL)
  {
    fclose(fp);
    return -2;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
  {
    fclose(fp);
    return -3;
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return -4;
  }

  /* Set image attributes. */

  png_set_IHDR(
      png_ptr,
      info_ptr,
      bitmap->width,
      bitmap->height,
      depth,
      PNG_COLOR_TYPE_RGB,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT);

  /* Initialize rows of PNG. */
  row_pointers = (png_byte **)png_malloc(png_ptr, bitmap->height * sizeof(png_byte *));
  for (y = 0; y < bitmap->height; y++)
  {
    png_byte *row =
        (png_byte *)png_malloc(png_ptr, sizeof(uint8_t) * bitmap->width * pixel_size);
    row_pointers[y] = row;

    for (x = 0; x < bitmap->width; x++)
    {
      Pixel *pixel = pixel_at(bitmap, x, y);
      *row++ = pixel->R;
      *row++ = pixel->G;
      *row++ = pixel->B;
    }
  }

  /* Write the image data to "fp". */
  png_init_io(png_ptr, fp);
  png_set_rows(png_ptr, info_ptr, row_pointers);
  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  /* Free the occupied memory */
  for (y = 0; y < bitmap->height; y++)
  {
    png_free(png_ptr, row_pointers[y]);
  }
  png_free(png_ptr, row_pointers);

  return 0;
}

Bitmap *create_frame()
{
  Bitmap *b;
  b = (Bitmap *)malloc(sizeof(Bitmap));
  b->width = ITEMS;
  b->height = ITEMS;
  b->pixels = (Pixel *)malloc(b->width * b->height * sizeof(Pixel));

  return b;
}

void hueToPixel(Pixel *p, double h)
{
  double fr, fg, fb;
  fr = fmod(5.0 + h * 6.0, 6.0);
  fg = fmod(3.0 + h * 6.0, 6.0);
  fb = fmod(1.0 + h * 6.0, 6.0);

  uint8_t R, G, B;
  R = 255 * (1 - max(min3(fr, 4 - fr, 1), (double)0));
  G = 255 * (1 - max(min3(fg, 4 - fg, 1), (double)0));
  B = 255 * (1 - max(min3(fb, 4 - fb, 1), (double)0));

  p->R = R;
  p->G = G;
  p->B = B;
}

void delete_frame(Bitmap *b)
{
  free(b->pixels);
  free(b);
}

int calculate_iters(int x, int y, double exponent)
{
  const double MAXDIST = sqrt(X * X + Y * Y);
  const double re = (double)x / ITEMS * X - X / 2;
  const double im = (double)y / ITEMS * Y - Y / 2;
  double complex c = CMPLX(re, im);
  double complex z = 0;

  int iters = 0;

  while (++iters < MAXITERS && cabs(z) < MAXDIST)
    z = cpow(z, exponent) + c;

  return iters;
}

void populate_frame(Bitmap *b, double exponent, double offset)
{

  for (int y = 0; y < ITEMS; y++)
  {
    for (int x = 0; x < ITEMS; x++)
    {
      const int iters = calculate_iters(x, y, exponent);
      Pixel *p = pixel_at(b, x, y);

      if (iters == MAXITERS)
      {
        p->R = 0;
        p->G = 0;
        p->B = 0;
      }
      else
      {
        double percent = easeOut((double)iters / MAXITERS);
        double hue = fmod(percent * COLORSPAN + offset, 1);
        hueToPixel(p, hue);
      }
    }
  }
}

void *thread(void *args)
{
  Parameters *p = (Parameters *)args;

  char filename[50];
  sprintf(filename, "%s%07d.png", FRAMESFOLDER, p->count);

  Bitmap *frame = create_frame();
  populate_frame(frame, p->exponent, p->offset);
  save_png_to_file(frame, filename);
  delete_frame(frame);

  printf("\tgenerated frame %d\n", p->count);

  return 0;
}

int main()
{

  for (int j = 0; j < FRAMES; j += WORKERS)
  {
    printf("Generating frames %d-%d\n", j, j + WORKERS - 1);

    pthread_t threads[WORKERS];
    Parameters params[WORKERS];

    for (int i = 0; i < WORKERS; i++)
    {
      const double percent = easeInOut((double)(i + j) / FRAMES);
      params[i].count = i + j;
      params[i].exponent = percent * (MAXEXP - MINEXP) + MINEXP;
      params[i].offset = percent * COLORREPETITIONS;
      pthread_create(&threads[i], NULL, thread, &params[i]);
    }

    for (int i = 0; i < WORKERS; i++)
    {
      pthread_join(threads[i], NULL);
    }
  }

  printf("All done!\n");

  return 0;
}