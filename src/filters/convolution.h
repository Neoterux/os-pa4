#include "kernel.h"
#include <stdint.h>
#ifndef _CONVOLUTION_H_
#define _CONVOLUTION_H_ 1

typedef struct {
  const kernel_t kernel;
  double kernel_multiplier;
} kernel_config_t;

uint8_t apply_filter(const kernel_t kernel, const double multiplier, const uint8_t (* __src_matrix)[3]);

#endif
