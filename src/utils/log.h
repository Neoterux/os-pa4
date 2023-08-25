#ifndef _PA4LOG_H_
#define _PA4LOG_H_

/**
 * @brief Log an info message
 *
 * @param __message The message string
 * @param ... arguments
 */
void log_info(const char* __message, ...);

/**
 * @brief Log a warning message
 *
 * @param __message The message string
 * @param ... arguments
 */
void log_warn(const char* __message, ...);

/**
 * @brief Log an error message
 *
 * @param __message The message string
 * @param ... arguments
 */
void log_err(const char* __message, ...);

/**
 * @brief Log a debug message
 *
 * @param __message The message string
 * @param ... arguments
 */
void log_debug(const char* __message, ...);


#endif /* _PA4LOG_H_ */
