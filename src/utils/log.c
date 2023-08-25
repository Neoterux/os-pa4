#include "log.h"
#include "colors.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void log_info(const char *message, ...) {
  va_list argptr;
  va_start(argptr, message);
  char formatted[sizeof(ANSI_COLOR_BLUE) + strlen(message) + sizeof(ANSI_RESET_ALL)];
  strcpy(formatted, ANSI_COLOR_BLUE);
  strcat(formatted, message);
  strcat(formatted, ANSI_RESET_ALL);
  vprintf(formatted, argptr);
  va_end(argptr);
}

void log_warn(const char *message, ...) {
  va_list argptr;
  va_start(argptr, message);
  char formatted[sizeof(ANSI_COLOR_YELLOW) + strlen(message) + sizeof(ANSI_RESET_ALL)];
  strcpy(formatted, ANSI_COLOR_YELLOW);
  strcat(formatted, message);
  strcat(formatted, ANSI_RESET_ALL);
  vprintf(formatted, argptr);
  va_end(argptr);
}

void log_err(const char *message, ...) {
  va_list argptr;
  va_start(argptr, message);
  char formatted[sizeof(ANSI_COLOR_RED) + strlen(message) + sizeof(ANSI_RESET_ALL)];
  strcpy(formatted, ANSI_COLOR_RED);
  strcat(formatted, message);
  strcat(formatted, ANSI_RESET_ALL);
  vprintf(formatted, argptr);
  va_end(argptr);
}

void log_debug(const char *message, ...) {
  #ifdef DEBUG
  va_list argptr;
  va_start(argptr, message);
  char formatted[sizeof(ANSI_COLOR_GREEN) + strlen(message) + sizeof(ANSI_RESET_ALL)];
  strcpy(formatted, ANSI_COLOR_GREEN);
  strcat(formatted, message);
  strcat(formatted, ANSI_RESET_ALL);
  vprintf(formatted, argptr);
  va_end(argptr);
  #endif
}
