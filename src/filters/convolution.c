#include "convolution.h"
#include <stdint.h>

uint8_t apply_filter(const kernel_config_t kernel_settings, const uint8_t (*src_matrix)[3]) {
  uint8_t accumulator = 0;
  #define kernel(i, j) kernel_settings.kernel[i][j]
  #define multipler kernel_settings.multipler

  return accumulator;
}
