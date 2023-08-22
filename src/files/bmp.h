#ifndef _BMP_H_
#define _BMP_H_ 1

#include "image.h"

/**
 * @brief Read the given file path and try to decode image, if the image
 *  is not a BMP image, then an error will be raised.
 * 
 * @param __src The source path of the image
 * @param __out The target pointer where the data will be 
 * @return size_t 
 */
size_t read_bmp_image(const char* restrict __src, BMPImage* restrict __out);

int32_t bmp_image_width(BMPImage* restrict __image);
int32_t bmp_image_height(BMPImage* restrict __image);

int bmp_header_is_valid(BMPImage* restrict __src);

size_t write_bmp_image(const char* restrict __src, BMPImage* restrict __out);

#endif
