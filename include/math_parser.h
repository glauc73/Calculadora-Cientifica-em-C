#ifndef _MATH_PARSER_H
#define _MATH_PARSER_H
#include <ctype.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265
#endif

#ifndef dx
#define dx 1e-6
#endif

#ifndef M_e
#define M_e 2.71828182
#endif

#ifndef NUM_FUNCTIONS
#define NUM_FUNCTIONS 13
#endif

#ifndef SIZE_ARR
#define SIZE_ARR(X) sizeof(X) / sizeof(X[0])
#endif

void strlower(char *str);
int lenFunc(char *exp);
double PI_(char **exp);
double euller_number(char **exp);
double parse_sqrt(char **exp); 
double parse_cbrt(char **exp); 
double parse_tan(char **exp);
double parse_sen(char **exp);
double parse_cos(char **exp);
double parse_arctan(char **exp);
double parse_arcsen(char **exp);
double parse_arccos(char **exp);
double parse_ln(char **exp);
double parse_log(char **exp);
double parse_bracket(char **exp);
double parsepow(char **exp);
double parsefactor(char **exp);
double parseterm(char **exp);
double parseexp(char **exp);
double eval_X(char *exp, double X);
double eval(char *exp);
bool eval_bool(char *exp);

typedef enum
{
	DEGREE = 180,
	RAD = 314
} angle_type;

typedef struct{
	const char *name;
	double (*call)(char **);
} func_map;


#endif