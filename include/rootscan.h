#ifndef _ROOTSCAN_
#define _ROOTSCAN_

#ifndef dx
#define dx 1e-6
#endif

#ifndef EPSILON
#define EPSILON 1e-8
#endif

#ifndef NEWTON_ITERATIONS
#define NEWTON_ITERATIONS 128
#endif

#ifndef TOLERANCE_ROOT
#define TOLERANCE_ROOT 1e-7
#endif

#ifndef JUMP_X0
#define JUMP_X0 2
#endif

#ifndef DBLCMP
    #define DBLCMP(X, Y) ( ((X) - (Y)) < dx )
#endif
#include <math.h>
#include <float.h>
#include <limits.h>
#include <stdlib.h>
#include "config.h"

int comp(const void *a, const void *b);
double derivate(char *exp, double value);
size_t select_root(char *func, MathExpression* exp);
int filter_root(MathExpression* exp,  double *arr, size_t range_size);
double newton_method(char *exp, double x0);

#endif