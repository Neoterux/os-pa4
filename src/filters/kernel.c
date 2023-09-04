#include "kernel.h"
#include <stdint.h>
#include <sys/types.h>

const double GAUSSIAN_BLUR_3X3_KERNEL_MULTIPLIER = 1.0/16.0;

const double GAUSSIAN_BLUR_5X5_KERNEL_MULTIPLIER = -1.0/256.0;

const double BOX_BLUR_KERNEL_MULTIPLIER = 1.0/9.0;

const uint8_t GAUSSIAN_BLUR_3X3_KERNEL[3][3] = {
  { 1, 2, 1 },
  { 2, 4, 2 },
  { 1, 2, 1 },
};

const uint8_t BOX_BLUR_KERNEL[3][3] = {
  { 1, 1, 1 },
  { 1, 1, 1 },
  { 1, 1, 1 },
};
