#include "kernel.h"
#include <stdint.h>
#include <sys/types.h>

const double GAUSSIAN_BLUR_3X3_KERNEL_MULTIPLIER = 1.0/16.0;
const int GAUSSIAN_BLUR_3X3_KERNEL_FACTOR = 16;

const double GAUSSIAN_BLUR_5X5_KERNEL_MULTIPLIER = -1.0/256.0;

const double BOX_BLUR_KERNEL_MULTIPLIER = 1.0/9.0;
const int BOX_BLUR_KERNEL_FACTOR = 9;

const int_fast16_t GAUSSIAN_BLUR_3X3_KERNEL[3][3] = {
  { 1, 2, 1 },
  { 2, 4, 2 },
  { 1, 2, 1 },
};

const int_fast16_t BOX_BLUR_KERNEL[3][3] = {
  { 1, 1, 1 },
  { 1, 1, 1 },
  { 1, 1, 1 },
};

// const int_fast16_t RIDGE_KERNEL[3][3] = {
//   { 0, -1, 0 },
//   { -1, 4, -1 },
//   { 0, -1, 0 }
// };
// const int_fast16_t RIDGE_KERNEL[3][3] = {
//   { 0, 1, 0 },
//   { 1, -4, 1 },
//   { 0, 1, 0 }
// };

const int_fast16_t RIDGE_KERNEL[3][3] = {
  { 1, 0, -1 },
  { 2, 0, -2 },
  { 1, 0, -1 }
};
