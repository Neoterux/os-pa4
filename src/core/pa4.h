/**
 * @file pa4.h
 * @author Neoterux (labfernandez2014@gmail.com)
 * @brief This file contains functions used for the executable itself rather than the solution.
 * @version 0.1
 * @date 2023-08-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef PA_4_H
#define PA_4_H 1
#define _STR2(s) #s
#define _STR(s) _STR2(s)
#define PA4_VERSION "v" _STR(EXEC_VERSION)

void usage(char *exec);

void version(char *exec);

#endif
