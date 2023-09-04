#include <CUnit/Basic.h>
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <assert.h>
#include <stdint.h>
#include "../src/filters/kernel.h"
#include "../src/filters/convolution.h"

/**
 * @brief This is the portion of the pixel extracted from the image
 * there are NOT the real image, only the fixed data previous apply
 * to image.
 *
 */
uint_fast8_t BASE_IMAGE[3][3] = {
  { 1, 2, 3 },
  { 4, 5, 6 },
  { 7, 8, 9 },
};

uint_fast8_t BASE_IMAGE_OVERFLOW[3][3] = {
  { 0xff, 0x09, 0xab },
  { 0x33, 0x4a, 0xbd },
  { 0x00, 0x5f, 0xfe },
};

int init_suite(void) {
  printf("--== [ Starting Tests ] ==--\n");
  return 1;
}

int clean_suite(void) {
  return 1;
}

void test_convolution_box(void) {

  // kernel_config_t config = { .kernel= BOX_BLUR_KERNEL, .kernel_multiplier= BOX_BLUR_KERNEL_MULTIPLIER};
  uint8_t result = apply_filter(BOX_BLUR_KERNEL, BOX_BLUR_KERNEL_MULTIPLIER, BASE_IMAGE);

  CU_ASSERT_EQUAL(result, 0x09);
}

void test_gaussian_filter(void) {

}

void test_border_filter(void) {

}


int main(void) {
  CU_pSuite psuite = NULL;
  if (CUE_SUCCESS != CU_initialize_registry()) {
    return CU_get_error();
  }

  psuite = CU_add_suite("Suite 1 - Convolution Tests", init_suite, clean_suite);
  if (NULL == psuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (
    (NULL == CU_add_test(psuite, "Testing box filter", test_convolution_box))
  ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();

    // printf(">>> Launching test for convolution\n");
    // printf("| Testing 'box' filter: \n");
    // test_convolution_box();

    // printf("| Testing 'gaussian' filter: \n");
    // test_gaussian_filter();

    // printf("| Testing 'border' filter: \n");
    // test_border_filter();

    // printf(">>> Testing finished \n");
}
