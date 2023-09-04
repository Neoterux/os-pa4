#include "bmp.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../utils/log.h"
extern int image_errno;

#ifdef DEBUG
void print_header(BMPFileHeader *header) {
  log_debug("** Header data **\n");
  log_debug("Signature: %x\n", header->signature);
  log_debug("File size: %d\n", header->file_size);
  log_debug("Reserved1: %d\n", header->reserved1);
  log_debug("Reserved2: %d\n", header->reserved2);
  log_debug("Offset: %d\n", header->offset);
  log_debug("DIB Header size: %d\n", header->dib_header_size);
  log_debug("Width: %d\n", header->width_px);
  log_debug("Height: %d\n", header->height_px);
  log_debug("Num planes: %d\n", header->num_planes);
  log_debug("Bits per pixel: %d\n", header->bits_per_pixel);
  log_debug("Compression: %d\n", header->compression);
  log_debug("Image size: %d\n", header->image_size_bytes);
  log_debug("X resolution: %d\n", header->x_resolution_ppm);
  log_debug("Y resolution: %d\n", header->y_resolution_ppm);
  log_debug("Num colors: %d\n", header->num_colors);
  log_debug("Important colors: %d\n", header->important_colors);
  log_debug("** End of header data **\n");

}
#endif

BMPImage* read_bmp_image(const char *restrict src, size_t* _isout) {
  // Open the file and read the content
  FILE *fp = fopen(src, "rb");
  if (fp == NULL) {
    image_errno = IMAGE_ERRNO_FILE_NOT_FOUND;
    return NULL;
  }
  // Read the file header
  BMPFileHeader file_header;
  int readbytes = fread(&file_header, sizeof(BMPFileHeader), 1, fp);
  if (readbytes != 1) {
    image_errno = IMAGE_DECODE_HEADER_ERROR;
    return NULL;
  }
  // Check if the file is a valid BMP image
  if (file_header.signature != 0x4D42) {
    image_errno = IMAGE_INVALID_BMP_IMAGE;
    return NULL;
  } else if (file_header.bits_per_pixel != TRUECOLOR) {
    image_errno = IMAGE_NOT_SUPPORTED_BPP;
    return NULL;
  }
  else if (file_header.num_planes != 1) {
    image_errno = IMAGE_BMP_NOT_SUPPORTED_PLANES;
    return NULL;
  } else if (file_header.compression != 0) {
    image_errno = IMAGE_BMP_NOT_SUPPORTED_COMPRESSION;
    return NULL;
  }
  #ifdef DEBUG
  print_header(&file_header);
  #endif
  //
  BMPImage *image = malloc(sizeof(BMPImage));
  image->bytes_per_pixel = file_header.bits_per_pixel / 8;
  image->normalized_height = abs(file_header.height_px);
  image->normalized_width = abs(file_header.width_px);
  image->content.pixel_matrix = NULL;
  image->header = file_header;
  // memcpy(&image.header, &file_header, sizeof(BMPFileHeader));
  Pixel **content = malloc((image->normalized_height) * sizeof(Pixel*));
  const size_t total_pixels = image->normalized_height * image->normalized_width;
  size_t pixels_read = 0;

  for (int pixel_row = 0; pixel_row < image->normalized_height; pixel_row++) {
    content[pixel_row] = malloc((image->normalized_width) * sizeof(Pixel));
    for (int pixel_col = 0; pixel_col < image->normalized_width; pixel_col++) {
      Pixel pixel;
      readbytes = fread(&pixel, image->bytes_per_pixel, 1, fp);
      if (readbytes != 1) {
        image_errno = IMAGE_DECODE_HEADER_ERROR;
        return NULL;
      }
      pixels_read++;
      // memcpy(&content[pixel_row][pixel_col], &pixel, sizeof(Pixel));
      content[pixel_row][pixel_col] = pixel;
      if (pixels_read % 255 == 0)
        log_info("\rloading image: %lf%%", (double)pixels_read / (double) total_pixels * 100.0);
    }
  }
  image->content.pixel_matrix = content;
  log_info("\r\nBMP Image readed [%lld pixels | %lld bytes]\n", pixels_read, pixels_read * image->bytes_per_pixel);
  getc(fp);
  if (feof(fp)) {
    log_info("End of file reached\n");
  } else {
    size_t bytes_remaining = file_header.file_size - ftell(fp);
    log_err("Error when reading the file [left %lld bytes]\n", bytes_remaining);
  }


  return image;
}


size_t write_bmp_image(const char *restrict src, BMPImage *restrict out) {
    return 0;
}

size_t get_required_space_bmp(const BMPImage *restrict __src) {
  if (__src == NULL) {
    log_err("Trying to guess the required space of a NULL image\n");
    return 0;
  }
  // size_t accumulated_size = sizeof(BMPImage);
  // The bellow is to hold all the pixel pointers
  // accumulated_size += sizeof(Pixel*) * __src->normalized_height;
  // The bellow is to hold all the pixel matrix
  // accumulated_size += __src->normalized_height * __src->normalized_width * sizeof(Pixel);
  return __src->header.file_size;
}

int copy_content_to_shared_memory_bmp(int shm_id, BMPImage *restrict __dest, const BMPImage *restrict __source) {
  return 0;
}

int xd = 700;

int write_on_bmp_image(void *shmm, const BMPImage *restrict __src, Pixel pixel, int row, int col) {
  if (shmm == NULL) {
    log_err("Trying to write into an invalid shared memory pointer");
    exit(EXIT_FAILURE);
  }
  size_t offset = sizeof(BMPFileHeader);
  int bpp = __src->bytes_per_pixel;
  offset += col * bpp; // Go to the horizontal address
  offset += (row * __src->normalized_width) * bpp;

  if (xd-- > 0) {
    log_warn(
      "\n-----\n"
      ":: height: %u  ^  width: %u ^  bpp: %d\n"
      "The result of [addr: %p] [r: %d | c:%d] ==> [offset: %ld(%p)] \n"
      "result: %p\n"
      "====\n",__src->normalized_height, __src->normalized_width, bpp, shmm, row, col, offset, offset, offset + shmm);
  }

  memcpy(shmm + offset, &pixel, bpp);
  return 0;
}
