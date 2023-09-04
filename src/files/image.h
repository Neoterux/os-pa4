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

// Define an errno-like varible for image decoding
extern int image_errno;

// define the constants for the image errno
#define IMAGE_ERRNO_UNKNOWN_TYPE 1
#define IMAGE_ERRNO_INVALID_FILE 2
#define IMAGE_ERRNO_INVALID_HEADER 3
#define IMAGE_ERRNO_INVALID_DIB_HEADER 4
#define IMAGE_ERRNO_INVALID_FILE_HEADER 5
#define IMAGE_ERRNO_INVALID_IMAGE 6
#define IMAGE_ERRNO_NOT_SUPPORTED 7
#define IMAGE_INVALID_BMP_IMAGE 8
#define IMAGE_ERRNO_FILE_NOT_FOUND 9
#define IMAGE_DECODE_HEADER_ERROR 10
#define IMAGE_NOT_SUPPORTED_BPP 11
#define IMAGE_BMP_NOT_SUPPORTED_PLANES 12
#define IMAGE_BMP_NOT_SUPPORTED_COMPRESSION 13


typedef struct {
  uint8_t blue;
  uint8_t green;
  uint8_t red;
  uint8_t alpha;
} __attribute__((__packed__)) Pixel;

// Define a union for the image content
typedef union {
  Pixel **pixel_matrix;
  uint8_t **raw_data;
} ImageContent;

// define depth for the image as enum
typedef enum {
  DEPTH_1 = 1,
  DEPTH_4 = 4,
  DEPTH_8 = 8,
  HICOLOR = 16,
  TRUECOLOR = 24,
  DEPTH_32 = 32,
} ImageDepth;

// typedef union {
//   Pixel **pixel_matrix;
//   uint8_t **raw_data;
// } ImageContent;

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
  ImageDepth bits_per_pixel: 16;
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
  int normalized_width;
  int bytes_per_pixel;
  ImageContent content;
} BMPImage;

typedef enum {
  UNKNOWN = 0,
  BITMAP,
  PNG,
  JPEG,
  GIF,
  WEBP,
  TIFF,
} ImageType;

typedef enum {
  BLUE = 0,
  GREEN,
  RED,
} channel_t;

typedef struct {
  /**
   * The type of the image
   */
  ImageType type;
  uint32_t width, height;
  /**
   * The Image Pointer that contains the image data
   */
  void *content;
} Image;

/**
 * @brief Reads the content of a file and return the decoded image.
 *
 * @param dest_file The filepath for the target image
 * @param image_size The target variable where the image size will be stored
 * @return Image The size of the image.
 */
Image* read_image(const char *dest_file, size_t *image_size);

/**
 * @brief Write the contents of a given image on the target file.
 *
 * @param dest_file The file where the image will be written.
 * @param __src The source of the image data that will be written
 * @return size_t
 */
size_t write_image(const char *dest_file, const Image *restrict __src);

Pixel **get_pixel_matrix(Image *image);

size_t get_required_space(const Image *image);

/**
 * @brief Copy the data already parsed from the given source into the shared memory
 * to share the pixel across all the process
 *
 * @param shm_id The id of the shared memory
 * @param holder The already attached data on shared memory to complete
 * @param __source The source of the data
 * @return int result of the operation
 */
int copy_content_to_shared_memory(int shm_id, Image *holder, Image *restrict __source);

/**
 * @brief Copy the header data of the image and returns the address where the image pixels
 * should be written, return NULL if error.
 *
 * @param __output The output memory address.
 * @param __source The image that will be treat as source to copy.
 * @return void* The address where the pixel matrix (image content) start.
 */
void* write_image_header(void *restrict __output, const Image *restrict __source);

/**
 * @brief Get the pixel at object
 *
 * @param image The image to get the pixel
 * @param row the row of the pixel
 * @param col the col of the pixel
 * @return Pixel The pixel at the given location
 */
Pixel get_pixel_at(const Image *image, const int row, const int col);


void write_on_image(void* target, const Image *restrict __src,  Pixel pixel, int row, int col);

/**
 * @brief Print the message to the given image error
 *
 * @param msg The prefix of the message
 */
void iperror(const char *msg);

#endif
