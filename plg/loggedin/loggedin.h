#ifndef LOGGEDIN_H
#define LOGGEDIN_H

#include "bool.h"

typedef void (*fn)();

extern bool loggedin;
void onlogin(fn call);

#endif
