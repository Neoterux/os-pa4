#include "kernel.h"
#include <stdint.h>
#ifndef _CONVOLUTION_H_
#define _CONVOLUTION_H_ 1

typedef struct {
  kernel_t *kernel;
  double kernel_multiplier;
} kernel_config_t;

uint8_t apply_filter(const kernel_config_t kernel_settings, const uint8_t (* __src_matrix)[3]);

#endif
