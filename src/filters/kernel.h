/**
 * @file kernel.h Blur Filters configurations
 * @author Neoterux (labfernandez2014@gmail.com)
 * @brief This file contains the configuration used for
 *  the filters that wants to apply a blur on an image.
 * @version 0.1
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdint.h>
#ifndef _KERNEL_H_
#define _KERNEL_H_ 1

/**
 * @brief This is the type definition for a
 *  kernel used on filters
 *
 */
typedef int_fast16_t kernel_t[3][3];


/**
 * @brief This is the multipler that is applied after
 *  matrix multiplication, this is 1/16.
 *
 */
extern const double GAUSSIAN_BLUR_3X3_KERNEL_MULTIPLIER;

/**
 * @brief This is the multipler that is applied after
 *  matrix multiplication, this is -1/256.
 *
 */
extern const double GAUSSIAN_BLUR_5X5_KERNEL_MULTIPLIER;

/**
 * @brief This is the multipler that is applied after
 *  matrix multiplication, this is 1/9.
 *
 */
extern const double BOX_BLUR_KERNEL_MULTIPLIER;

/**
 * @brief Kernel for the gaussian filter.
 *
 */
extern const int_fast16_t GAUSSIAN_BLUR_3X3_KERNEL[3][3];

/**
 * @brief Kernel for the box blur
 *
 */
extern const int_fast16_t BOX_BLUR_KERNEL[3][3];

/**
 * @brief Kernel for edge detection;
 *
 */
extern const int_fast16_t RIDGE_KERNEL[3][3];

extern const int GAUSSIAN_BLUR_3X3_KERNEL_FACTOR;

extern const int BOX_BLUR_KERNEL_FACTOR;


#endif /* _KERNEL_H_ */
