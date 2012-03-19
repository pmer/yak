#include <pthread.h>
#include "thread.h"

int nthreads = 1;

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void mutex_on()
{
	if (nthreads > 1)
		pthread_mutex_lock(&mut);
}

void mutex_off()
{
	if (nthreads > 1)
		pthread_mutex_unlock(&mut);
}
