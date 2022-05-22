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
#include <time.h>     /* Used for time measurement */

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

const double Y = 5;                           // height of the mandelbrot set
const double X = 5;                           // width of the mandelbrot set
const double MIN_EXP = 1;                     // minimum exponent
const double MAX_EXP = 10;                    // maximum exponent
const char *FRAMES_FOLDER = "output/frames/"; // folder containing all the frames
const int PALETTE_SCL = 64;                   // scale of the palette smoothing
const int PALETTE_SIZE = 5;                   // size of the palette

const Pixel PALETTE[] = {
    (Pixel){170, 255, 1},
    (Pixel){255, 143, 1},
    (Pixel){255, 0, 170},
    (Pixel){170, 0, 255},
    (Pixel){0, 170, 255},
};
Pixel SMOOTH_PALETTE[64 * 5]; // destination of the smoothed palette

const int FRAME_SIZE = 1000; // number of pixels in a single frame
const int FRAMES = 1800;     // number of frames in the video
const int THREADS_NUM = 8;   // number of threads
const int MAX_ITERS = 1024;  // maximum iterations for each pixel

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
 * @brief Polynomial easing in and out function
 *
 * @param x variable to ease
 * @param n degree of the polynomial
 * @return double
 */
double poly_ease(double x, double n)
{
  return x < 0.5 ? pow(2, n - 1) * pow(x, n) : 1 - pow(-2 * x + 2, n) / 2;
}

/**
 * @brief Map a value from one range to another
 *
 * @param x value to map
 * @param in_min old range minimum
 * @param in_max old range maximum
 * @param out_min new rang minimum
 * @param out_max new range maximum
 * @return double
 */
double map(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
 * @brief Create a smooth palette
 */
void create_palette()
{

  for (int i = 0; i < PALETTE_SIZE; i++)
  {
    int current = i;
    int next = i < PALETTE_SIZE - 1 ? i + 1 : 0;

    for (int j = 0; j < PALETTE_SCL; j++)
    {
      double percent = (double)j / (PALETTE_SCL - 1);
      double smooth = poly_ease(percent, 2);

      *(SMOOTH_PALETTE + i * PALETTE_SCL + j) = (Pixel){
          .R = (uint8_t)((1 - smooth) * (double)PALETTE[current].R + smooth * (double)PALETTE[next].R),
          .G = (uint8_t)((1 - smooth) * (double)PALETTE[current].G + smooth * (double)PALETTE[next].G),
          .B = (uint8_t)((1 - smooth) * (double)PALETTE[current].B + smooth * (double)PALETTE[next].B),
      };
    }
  }
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
int save_frame_to_file(Bitmap *bitmap, const char *path)
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
Bitmap *create_frame(int size)
{
  Bitmap *b;
  b = (Bitmap *)malloc(sizeof(Bitmap));
  b->width = size;
  b->height = size;
  b->pixels = (Pixel *)malloc(b->width * b->height * sizeof(Pixel));

  return b;
}

/**
 * @brief Convert the number of iterations to a colour from the palette
 *
 * @param iters number of iterations
 * @param p destination pixel
 */
void iters_to_pixel(int iters, Pixel *p)
{
  // calculate the palette index
  int index = iters % (PALETTE_SIZE * PALETTE_SCL);
  // assign colours
  p->R = SMOOTH_PALETTE[index].R;
  p->G = SMOOTH_PALETTE[index].G;
  p->B = SMOOTH_PALETTE[index].B;
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
  const double MAX_DIST = sqrt(X * X + Y * Y);          // max distance from origin
  const double re = (double)x / FRAME_SIZE * X - X / 2; // real part of c
  const double im = (double)y / FRAME_SIZE * Y - Y / 2; // imaginary part of c
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
  for (int y = 0; y < FRAME_SIZE; y++)
  {
    for (int x = 0; x < FRAME_SIZE; x++)
    {
      int iters = calculate_iters(x, y, exponent);
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
        iters += offset * (PALETTE_SIZE * PALETTE_SCL);
        iters_to_pixel(iters, p);
      }
    }
  }
}

/**
 * @brief Create a test file containing the smoothed palette
 *
 */
void test_palette()
{
  const char *path = "palette.png";
  const int frame_size = PALETTE_SIZE * PALETTE_SCL;
  Bitmap *b = create_frame(frame_size);

  for (int x = 0; x < frame_size; x++)
  {
    for (int y = 0; y < frame_size; y++)
    {
      Pixel *p = pixel_at(b, x, y);
      p->R = SMOOTH_PALETTE[x].R;
      p->G = SMOOTH_PALETTE[x].G;
      p->B = SMOOTH_PALETTE[x].B;
    }
  }
  save_frame_to_file(b, path);
  delete_frame(b);
  return;
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

  Bitmap *frame = create_frame(FRAME_SIZE);
  populate_frame(frame, p->exponent, p->offset);
  save_frame_to_file(frame, filename);
  delete_frame(frame);

  printf("\tgenerated frame %d\n", p->count);

  return 0;
}

int main()
{
  clock_t started;
  started = clock();

  // create folder if it doesn't exist
  printf("Creating folder %s\n", FRAMES_FOLDER);
  create_folder();

  // smooth colours
  printf("Smoothing colours\n");
  create_palette(SMOOTH_PALETTE);

  // test palette
  printf("Testing palette\n");
  test_palette();

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
      const double percent = poly_ease((double)(i + j) / FRAMES, 2);
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

  clock_t ended = clock();
  double time_spent = (double)(ended - started) / CLOCKS_PER_SEC;
  double fps = FRAMES / time_spent;
  printf("Time taken: %f seconds\n", time_spent);
  printf("FPS: %f\n", fps);

  return 0;
}