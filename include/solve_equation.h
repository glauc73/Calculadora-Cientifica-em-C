#ifndef _SOLVE_EQUATION_H
#define _SOLVE_EQUATION_H

#include "math_parser.h"
#include "gnrstack.h"
#ifndef M_PI
	#define M_PI 3.14159265
#endif

#ifndef M_e
	#define M_e 2.71828182
#endif

#ifndef dx
#define dx 1e-6
#endif

#define ROOT_MAX 64
#define BUFSIZE 512
typedef enum
{
	MULTIPLY_VAR = -1,
	NOT_VAR,
	EVAL = 8,
	EVAL_X,
	BOOLE,
	EQUATION,
	FORMAT_ERROR,
	INVALID_CHAR,
	NO_RIGHT,
	NO_LEFT,
	BRACKETS_ODD
} solve_type;

//tipo de expressões matematicas, guardando a expressão (input), o vetor com as raizes, a quantidade de raizes, o resultado
//o status, a variavel (signed char pq MULTIPLY_VAR = -1, e os ponteiros pro lado direito e esquerdo)
typedef struct {
	char input[BUFSIZE];
	double root[ROOT_MAX], count_root, result;
	solve_type status;
	signed char var;
	struct side_equation{
		char* left;
		char* right;
	}side;
}MathExpression;

bool isoperat(char ch);
void removespace(char* exp);
char *func_composite(char *exp, const char *g_x);
int comp(const void *a, const void *b);
double derivate(char *exp, double value);
char invalid_char(char *iterator);
void Create_Function(MathExpression* exp);
int isIdentity(MathExpression* exp);
double solve(MathExpression* exp);
double newton_method(char *exp, double x0);
int filter_root(MathExpression* exp, double *arr, size_t range_size);
size_t select_root(char *func, MathExpression* exp);
void add_product(char *exp);
char checkvar(char *exp);
double get_eval(const char* msg);

extern signed char var;
extern const func_map functions[NUM_FUNCTIONS];

#endif