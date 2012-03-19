#ifndef THREAD_H
#define THREAD_H

#include "bool.h"

/**
 * Enable mutexes support if nthreads > 1.
 */
extern int nthreads;

/**
 * Global mutex.
 *
 * Any procedure that wants to access or change global IRC data structures
 * should lock. The main thread locks whenever a line is read from a socket.
 *
 * Since IRC is relatively cheap on processing power, we don't consider the
 * possiblity of multiple threads processing data at once.
 */
void mutex_on();
void mutex_off();

#endif
