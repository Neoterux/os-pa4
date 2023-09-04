#include "image.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include "bmp.h"
#include "../utils/log.h"

int image_errno = 0;

ImageType identify_image_type(const char *dest_file) {
  // read part of the file and try to identify the type
  FILE *fp = fopen(dest_file, "rb");
  if (fp == NULL) {
    image_errno = 1;
    return UNKNOWN;
  }
  // Try to infer the type of the image from the signature
  uint8_t signature[8];
  fread(signature, sizeof(uint8_t), 8, fp);
  fclose(fp);
  if (signature[0] == 0x89 && signature[1] == 0x50 && signature[2] == 0x4E && signature[3] == 0x47 && signature[4] == 0x0D && signature[5] == 0x0A && signature[6] == 0x1A && signature[7] == 0x0A) {
    return PNG;
  } else if (signature[0] == 0xFF && signature[1] == 0xD8 && signature[2] == 0xFF) {
    return JPEG;
  } else if (signature[0] == 0x47 && signature[1] == 0x49 && signature[2] == 0x46 && signature[3] == 0x38 && (signature[4] == 0x37 || signature[4] == 0x39) && signature[5] == 0x61) {
    return GIF;
  } else if (signature[0] == 0x52 && signature[1] == 0x49 && signature[2] == 0x46 && signature[3] == 0x46) {
    return WEBP;
  } else if (signature[0] == 0x49 && signature[1] == 0x49 && signature[2] == 0x2A && signature[3] == 0x00) {
    return TIFF;
  } else if (signature[0] == 0x42 && signature[1] == 0x4D) {
    return BITMAP;
  } else {
    return UNKNOWN;
  }
}

Image* read_image(const char *dest_file, size_t *image_size) {
  Image *image = (Image*) malloc(sizeof(Image));
  // Try to identity the type of the image and decode by it
  image->type = identify_image_type(dest_file);
  image->content = NULL;
  if (image->type == UNKNOWN) {
    image_errno = IMAGE_ERRNO_UNKNOWN_TYPE;
    free(image);
    return NULL;
  }
  // Now decode by the type
  if (image->type == BITMAP) {
    log_debug("The source was identified as a bitmap image\n");
    BMPImage *img = read_bmp_image(dest_file, NULL);
    image->content = img;
    image->height = img->normalized_height;
    image->width = img->normalized_width;

  } else {
    image_errno = IMAGE_ERRNO_UNKNOWN_TYPE;
    free(image);
    return NULL;
  }

  return image;
}

void iperror(const char *msg) {
  char* error_msg = NULL;
  switch(image_errno) {
    case IMAGE_ERRNO_FILE_NOT_FOUND:
      error_msg = "File not found";
      break;
    case IMAGE_DECODE_HEADER_ERROR:
      error_msg = "Error when decoding the header";
      break;
    case IMAGE_INVALID_BMP_IMAGE:
      error_msg = "Invalid BMP image";
      break;
    case IMAGE_NOT_SUPPORTED_BPP:
      error_msg = "Not supported bits per pixel";
      break;
    case IMAGE_ERRNO_UNKNOWN_TYPE:
      error_msg = "Unknown image type";
      break;
    case IMAGE_BMP_NOT_SUPPORTED_COMPRESSION:
      error_msg = "Not supported compression";
      break;
    case IMAGE_BMP_NOT_SUPPORTED_PLANES:
      error_msg = "Not supported planes";
      break;
    default:
      error_msg = "Unknown error";
      break;
  }
  log_err("%s: %s\n", msg, error_msg);
}

size_t get_required_space(const Image *image) {
  if (image->type == BITMAP) {
    return get_required_space_bmp(image->content);
  } else {
    return 0;
  }
}

int copy_content_to_shared_memory(int shm_id, Image *holder, Image *restrict __source) {
  if (holder->type == BITMAP) {
    // Attach a new bmp image to the shared memory
    // BMPImage *bmp_image = shmat(shm_id, NULL, 0);
    return copy_content_to_shared_memory_bmp(shm_id, holder->content, __source->content);
  } else {
    return 0;
  }
}

void* write_image_header(void *restrict __output, const Image *restrict __source) {
  if (__output == NULL) {
    log_err("Trying to copy header on null memory region\n");
    exit(EXIT_FAILURE);
  }
  if (__source == NULL) {
    log_err("Trying to copy a NULL Image\n");
    exit(EXIT_FAILURE);
  }
  if (__source->type == BITMAP) {
    BMPImage *src = __source->content;
    memcpy(__output, &src->header, sizeof(BMPFileHeader));
    return __output + sizeof(BMPFileHeader);
  }
  return NULL;
}

Pixel get_pixel_at(const Image *image, const int row, const int col) {
  Pixel output = { 0, 0, 0, 0 };
  if (image->type == BITMAP) {
      BMPImage *img = image->content;
      output = img->content.pixel_matrix[row][col];
  }
  return output;
}


void write_on_image(void* target, const Image *restrict __src,  Pixel pixel, int row, int col) {
  if (__src->type == BITMAP) {
    write_on_bmp_image(target, __src->content, pixel, row, col);
  }
}
