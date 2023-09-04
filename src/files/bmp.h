#include <stddef.h>
#ifndef _BMP_H_
#define _BMP_H_ 1

#include "image.h"

/**
 * @brief Read the given file path and try to decode image, if the image
 *  is not a BMP image, then an error will be raised.
 *
 * @param __src The source path of the image
 * @param _isout The target pointer where the data will be
 * @return BMPImage* The image output
 */
 BMPImage* read_bmp_image(const char* restrict __src, size_t* _isout);
// size_t read_bmp_image(const char* restrict __src, BMPImage* restrict __out);

int32_t bmp_image_width(BMPImage* restrict __image);
int32_t bmp_image_height(BMPImage* restrict __image);


/**
 * @brief Save the given input on the target location.
 *
 * @param __src The target image destination
 * @param __out The source of data to write
 * @return size_t The total size of bytes written
 */
size_t write_bmp_image(const char* restrict __src, BMPImage* restrict __out);

/**
 * @brief Get the required space bmp object
 *
 * @param __src The object source
 * @return size_t The result of needed space (on memory) to hold all the structure
 */
size_t get_required_space_bmp(const BMPImage *restrict __src);

int copy_content_to_shared_memory_bmp(int shm_id, BMPImage *restrict __dest, const BMPImage *restrict __source);

 /**
  * @brief Write the pixel given the BMP method to encode pixel, into the shared memory.
  *
  * @param shmm The pointer of the shared memory to write into
  * @param __src The source of the image.
  * @param pixel The target pixel to write
  * @param row The target row of the pixel
  * @param col the column of the pixel
  * @return int
  */
int write_on_bmp_image(void *shmm, const BMPImage *restrict __src, Pixel pixel, int row, int col);

#endif
