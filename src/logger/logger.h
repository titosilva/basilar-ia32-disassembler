#pragma once

#include <stdio.h>

#if DEBUG
#define log_debug(...) fprintf(stderr, __VA_ARGS__)
#else
#define log_debug(...)
#endif