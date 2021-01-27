/**
 * This file defines a variety of functions enabling the user to output data in
 * a thread safe manner. Using printf is unsafe within the framework and will
 * result in undefined behavior. Specifically you will get deadlocks that
 * freeze the program. Anything you would like to output to stdout should be
 * sent by one of the below functions.
 */

#ifndef OS_THREADING_LOGGER_H
#define OS_THREADING_LOGGER_H

/**
 * Logs the given string to stdout and adds a new line.
 *
 * @param str The string to output to the log.
 */
void logLine(const char* str);

/**
 * Logs the given string to stdout.
 *
 * @param str The string to output to the log.
 */
void log(const char* str);

/**
 * Logs the given line to stdout if verbose is turned on.
 *
 * @param str The string to output to the log.
 */
void verboseLog(const char* str);

/**
 * Turns verbose logging on or off.
 *
 * @param val If true, turn verbose logging on, if false, turn verbose logging
 * off.
 */
void setVerbose(bool val);
#endif  // OS_THREADING_LOGGER_H
