//#define DEBUG

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <png.h>      /* Library for png creation */
#include <complex.h>  /* Standard Library of Complex Numbers */
#include <math.h>     /* Standard Library of math */
#include <sys/stat.h> /* Used for mkdir */
#include <tgmath.h>   /* Used for log2 */

const double X = 5;                           // width of the mandelbrot set
const double Y = 5;                           // height of the mandelbrot set
const double MIN_EXP = 1;                     // minimum exponent
const double MAX_EXP = 10;                    // maximum exponent
const double COLOUR_RANGE = 0.25;             // hue range within a frame
const char *FRAMES_FOLDER = "output/frames/"; // folder containing all the frames

// #define DEBUG

#ifdef DEBUG
const int PIXELS = 1000;
const int FRAMES = 10;
const int THREADS_NUM = 8;
const int MAX_ITERS = 1000;
#else
const int PIXELS = 1000;    // number of pixels in a single frame
const int FRAMES = 900;     // number of frames in the video
const int THREADS_NUM = 8;  // number of threads
const int MAX_ITERS = 1024; // maximum iterations for each pixel
#endif

/**
 * @brief Struct containing a single coloured pixel
 *
 */
typedef struct
{
  uint8_t R; // red channel
  uint8_t G; // green channel
  uint8_t B; // blue channel
} Pixel;

/**
 * @brief Struct containing an entire image (frame)
 *
 * Memory must be allocated for the image
 */
typedef struct
{
  Pixel *pixels; // array of pixels
  size_t width;  // width of the image
  size_t height; // height of the image
} Bitmap;

/**
 * @brief Struct containing the parameters for a single thread
 *
 */
typedef struct
{
  uint32_t count;  // index of the frame
  double exponent; // exponent of the mandelbrot set
  double offset;   // offset of the hue
} Parameters;

/**
 * @brief Max between 2 numbers
 *
 * @param a
 * @param b
 * @return double
 */
double max(double a, double b)
{
  if (a > b)
    return a;
  return b;
}

/**
 * @brief Min between 2 numbers
 *
 * @param a
 * @param b
 * @return double
 */
double min(double a, double b)
{
  if (a < b)
    return a;
  return b;
}

/**
 * @brief Min between 3 numbers
 *
 * @param x
 * @param y
 * @param z
 * @return double
 */
double min3(double x, double y, double z)
{
  return min(min(x, y), z);
}

/**
 * @brief Quadratic easing in and out function
 *
 * @param x in range [0, 1]
 * @return double  eased in and out number in range [0, 1]
 */
double ease_percent(double x)
{
  return x < 0.5 ? 2 * pow(x, 2) : 1 - pow(-2 * x + 2, 2) / 2;
}

/**
 * @brief Create the folder for the frames.
 * Should work both with windows and GNU/Linux
 *
 * @return int 0 if successful, -1 otherwise
 */
int create_folder()
{
#ifdef __linux__
  return mkdir(FRAMES_FOLDER, S_IRWXU);
#else
  return mkdir(FRAMES_FOLDER);
#endif
}

/**
 * @brief Get a pixel from the bitmap.
 * Adapted from https://www.lemoda.net/c/write-png/
 *
 * @param bitmap image
 * @param x x coordinate
 * @param y y coordinate
 * @return Pixel*
 */
Pixel *pixel_at(Bitmap *bitmap, int x, int y)
{
  return bitmap->pixels + bitmap->width * y + x;
}

/**
 * @brief Write bitmap to file.
 * Adapted from https://www.lemoda.net/c/write-png/
 *
 * @param bitmap image to be saved
 * @param path path to the file
 * @return int 0 if successful, -1 otherwise
 */
int save_png_to_file(Bitmap *bitmap, const char *path)
{
  FILE *fp;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  size_t x, y;
  png_byte **row_pointers = NULL;

  int pixel_size = 3;
  int depth = 8;

  // try to open the destination
  fp = fopen(path, "wb");
  if (!fp)
    return -1;

  // create an png image
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL)
  {
    fclose(fp);
    return -2;
  }

  // create info
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
  {
    fclose(fp);
    return -3;
  }

  // kinda confused about this one tbh
  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return -4;
  }

  // Set up image attributes
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

  // Initialize each row of the PNG.
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

  // Write image data to the pointed file
  png_init_io(png_ptr, fp);
  png_set_rows(png_ptr, info_ptr, row_pointers);
  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  // Free the previously allocated memory
  for (y = 0; y < bitmap->height; y++)
    png_free(png_ptr, row_pointers[y]);
  png_free(png_ptr, row_pointers);

  return 0;
}

/**
 * @brief Create a single frame
 *
 * @return Bitmap* pointer to the bitmap image
 */
Bitmap *create_frame()
{
  Bitmap *b;
  b = (Bitmap *)malloc(sizeof(Bitmap));
  b->width = PIXELS;
  b->height = PIXELS;
  b->pixels = (Pixel *)malloc(b->width * b->height * sizeof(Pixel));

  return b;
}

/**
 * @brief Convert hue and create a pixel
 *
 * @param p pixel to be converted
 * @param h hue, in range [0, 1]
 */
void hue_to_pixel(double h, Pixel *p)
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

/**
 * @brief Delete a frame
 *
 * @param b image to be deleted
 */
void delete_frame(Bitmap *b)
{
  free(b->pixels);
  free(b);
}

/**
 * @brief Calculate the number of iterations before the point diverges
 *
 * @param x x coordinate of the point
 * @param y y coordinate of the point
 * @param exponent mandelbrot set exponent
 * @return int number of iterations before the point diverges
 */
int calculate_iters(int x, int y, double exponent)
{
  const double MAX_DIST = sqrt(X * X + Y * Y);      // max distance from origin
  const double re = (double)x / PIXELS * X - X / 2; // real part of c
  const double im = (double)y / PIXELS * Y - Y / 2; // imaginary part of c
  double complex c = CMPLX(re, im);
  double complex z = 0;

  int iters = 0;

  while (++iters < MAX_ITERS && cabs(z) < MAX_DIST)
    z = cpow(z, exponent) + c; // mandelbrot set equation

  if (iters == MAX_ITERS)
    return iters;

  return max(0, iters + 1 + log(log(cabs(z))) / log(2));
}

/**
 * @brief Fill a frame with the mandelbrot set
 *
 * @param b image to be filled
 * @param exponent mandelbrot set exponent
 */
void populate_frame(Bitmap *b, double exponent, double offset)
{
  for (int y = 0; y < PIXELS; y++)
  {
    for (int x = 0; x < PIXELS; x++)
    {
      const int iters = calculate_iters(x, y, exponent);
      Pixel *p = pixel_at(b, x, y);

      // set to black if diverged
      if (iters >= MAX_ITERS)
      {
        p->R = 0;
        p->G = 0;
        p->B = 0;
      }
      else
      {
        const double h = fmod(fmod((double)iters / MAX_ITERS, COLOUR_RANGE) + offset, 1);
        hue_to_pixel(h, p);
      }
    }
  }
}

/**
 * @brief Single thread
 *
 * @param args arguments of the thread
 * @return void*
 */
void *thread(void *args)
{
  Parameters *p = (Parameters *)args;

  char filename[50];
  sprintf(filename, "%s%07d.png", FRAMES_FOLDER, p->count);

  Bitmap *frame = create_frame();
  populate_frame(frame, p->exponent, p->offset);
  save_png_to_file(frame, filename);
  delete_frame(frame);

  printf("\tgenerated frame %d\n", p->count);

  return 0;
}

int main()
{
  // create folder if it doesn't exist
  printf("Creating folder %s\n", FRAMES_FOLDER);
  create_folder();

  // here we go!
  printf("Attempting to generate %d frames.\n", FRAMES);

  for (int j = 0; j < FRAMES; j += THREADS_NUM)
  {
    printf("Generating frames %d-%d\n", j, (int)min(j + THREADS_NUM - 1, FRAMES));

    pthread_t threads[THREADS_NUM];
    Parameters params[THREADS_NUM];

    // divide the frames creation into threads
    for (int i = 0; i < THREADS_NUM; i++)
    {
      const double percent = ease_percent((double)(i + j) / FRAMES);
      params[i].count = i + j;
      params[i].exponent = percent * (MAX_EXP - MIN_EXP) + MIN_EXP;
      params[i].offset = percent;
      pthread_create(threads + i, NULL, thread, params + i);
    }

    // wait for completion of all threads
    for (int i = 0; i < THREADS_NUM; i++)
      pthread_join(threads[i], NULL);
  }

  printf("All done!\n");

  return 0;
}