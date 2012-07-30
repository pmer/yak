#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <stdarg.h>
#include <stdbool.h>

/**
 * info - display msg to user running yak
 */
void info(char* format, ...);

/**
 * err - display error msg to user running yak
 */
void err(char* format, ...);

/**
 * err - display error msg with errno information to user running yak
 */
void err_errno(char* format, ...);

/**
 * safe_shutdown - nicely shut down all running services without exitting
 */
void safe_shutdown();

/**
 * safe_shutdown_and_die - nicely shut down all running services, then exit
 */
void safe_shutdown_and_die(int code);

/**
 * add_shutdown_fn - have safe_shutdown call a function when it is called
 *
 * Functions are called in first-in-last-out order.
 */
void add_shutdown_fn(void (*fn)());

/**
 * die - print err msg, then exit immediately
 */
void die(char* msg);

/**
 * want_quit - request a safe shutdown
 *
 * When a part of the program wants to quit in a super polite manner, they can
 * toggle this flag. It's even more polite than calling safe_shutdown()
 * directly.
 *
 * When the program finishes its current iteration of the main IRC processing
 * loop, it will quit.
 */
extern bool want_quit;

#endif
