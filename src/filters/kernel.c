#include "kernel.h"

const double GAUSSIAN_BLUR_3X3_KERNEL_MULTIPLIER = 1.0/16.0;

const double GAUSSIAN_BLUR_5X5_KERNEL_MULTIPLIER = -1.0/256.0;

const double BOX_BLUR_KERNEL_MULTIPLIER = 1.0/9.0;

const kernel_t GAUSSIAN_BLUR_3X3_KERNEL = {
  { 1, 2, 1 },
  { 2, 4, 2 },
  { 1, 2, 1 },
};

const kernel_t BOX_BLUR_KERNEL = {
  { 1, 1, 1 },
  { 1, 1, 1 },
  { 1, 1, 1 },
};
