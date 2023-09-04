#include <emmintrin.h>
#pragma GCC push_options
#pragma GCC target("sse4.1", "sse4.2", "avx2")


#include "convolution.h"
#include <stdint.h>
#include "../utils/log.h"
#include "immintrin.h"

/**
* Convert a 3x3 matrix into a AVX2 vector
*/
#define _m256_matrix(matrix) _mm256_set_epi16(\
  matrix[0][0], matrix[0][1], matrix[0][2], matrix[1][0], matrix[1][1], matrix[1][2], \
  matrix[2][0], matrix[2][1], matrix[2][2],            0,            0,            0, \
             0,            0,            0,            0 \
  )

// __attribute__((target("default")))
uint8_t apply_filter_default(const kernel_t kernel, const double multiplier, const uint8_t (*src_matrix)[3]) {
  uint8_t accumulator = 0;
  log_debug("using the default calcuation method\n");
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      accumulator += src_matrix[i][j] * kernel[i][j];
    }
  }
  return accumulator * multiplier;
}


// based on https://gist.github.com/vermorel/7ad35212df44f3a79bca8ab5fe8e7622
// __attribute__((target("avx2")))
uint8_t apply_filter_avx2(const kernel_t kernel, const double multipler, const uint8_t (*src_matrix)[3]) {
  log_debug("using avx2 optimizations\n");
  const __m256i _mm_kernel = _m256_matrix(kernel);
  const __m256i src = _m256_matrix(src_matrix);
  const __m256i precalc = _mm256_mullo_epi16(_mm_kernel, src);
  #if defined(__clang__)
  __m128i vlow = _mm256_castsi256_si128(precalc);
  // Calculate the sum of all the avx vector on single value
  __m128i vhigh = _mm256_extracti128_si256(precalc, 1);
  // Sum the two parts of the avx register
  __m128i sum = _mm_add_epi16(vlow, vhigh);
  // Sum the two parts of the avx register
  sum = _mm_hadd_epi16(sum, sum);
  // Sum the two parts of the avx register
  sum = _mm_hadd_epi16(sum, sum);
  // Store the lowest part of the avx register
  return _mm_extract_epi16(sum, 0) * multipler;
  #elif defined(__GNUC__) || defined(__GNUG__) && defined(__AVX2__)
  // Calculate the sum of all the avx vector on single value
  const __m128i vlow = _mm256_castsi256_si128(precalc);
  // Calculate the sum of all the avx vector on single value
  const __m128i vhigh = _mm256_extracti128_si256(precalc, 1);
  // Sum the two parts of the avx register
  const __m128i sum = _mm_add_epi16(vlow, vhigh);
  // Sum the two parts of the avx register
  const __m128i sum2 = _mm_hadd_epi16(sum, sum);
  // Sum the two parts of the avx register
  const __m128i sum3 = _mm_hadd_epi16(sum2, sum2);
  // Store the lowest part of the avx register
  return _mm_extract_epi16(sum3, 0) * multipler;
  #else
  #error("NO Supported Compiler")
  #endif
}

__attribute__(( target("sse4.1,sse4.2") ))
uint_fast8_t apply_filter_sse4(const kernel_t kernel_settings, const double multiplier, const uint8_t (*src_matrix)[3]) {
  return 0;
}


uint8_t apply_filter(const kernel_t kernel, const double multipler, const uint8_t (*src_matrix)[3]) {
  __builtin_cpu_init();
  if (__builtin_cpu_supports("avx2")) {
    return apply_filter_avx2(kernel, multipler, src_matrix);
  } else if (__builtin_cpu_supports("sse4.1")) {
    return apply_filter_sse4(kernel, multipler, src_matrix);
  }
  return apply_filter_default(kernel, multipler, src_matrix);
}
#pragma GCC pop_options
