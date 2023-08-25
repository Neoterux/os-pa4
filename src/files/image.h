/**
 *
 * This file will contains the required data
 * for image processing, like decoding/encoding
 * read/write and more.
 *
 * The target is to support only Bitmap Images
 * but I'll try to at least support JPEG or PNG
 * with my own decoding functions.
 *
 * @author Neoterux
 * @created-at 2023-08-12
 */
#include <stddef.h>
#include <stdint.h>

#ifndef _PA4_IMAGE_H
#define _PA4_IMAGE_H 1

typedef struct {
  uint8_t blue;
  uint8_t green;
  uint8_t red;
  uint8_t alpha;
} __attribute__((__packed__)) Pixel;

typedef union {
  Pixel **pixel_matrix;
  uint8_t **raw_data;
} ImageContent;

typedef struct {
  int32_t header_size;
} DIB_Header;

typedef struct {
  uint16_t signature;
  uint32_t file_size;
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t offset;
  uint32_t dib_header_size;
  int32_t width_px;
  int32_t height_px;
  uint16_t num_planes;
  uint16_t bits_per_pixel;
  uint32_t compression;
  uint32_t image_size_bytes;
  int32_t x_resolution_ppm;
  int32_t y_resolution_ppm;
  uint32_t num_colors;
  uint32_t important_colors;
} __attribute__((__packed__)) BMPFileHeader;

typedef struct {
  BMPFileHeader header;
  int normalized_height;
  int bytes_per_pixel;
  ImageContent content;
} BMPImage;

typedef enum {
  BITMAP = 0,
} ImageType;

typedef struct {
  /**
   * The type of the image
   */
  ImageType type;
  /**
   * The Image Pointer that contains the image data
   */
  void *content;
} Image;

/**
 * @brief Reads the content of a file and return the decoded image.
 *
 * @param dest_file The filepath for the target image
 * @param target The target container where the data will be written.
 * @return size_t The size of the image.
 */
size_t read_image(const char *dest_file, Image *target);

/**
 * @brief Write the contents of a given image on the target file.
 *
 * @param dest_file The file where the image will be written.
 * @param __src The source of the image data that will be written
 * @return size_t
 */
size_t write_image(const char *dest_file, const Image *restrict __src);

Pixel **get_pixel_matrix(Image *image);

#endif
