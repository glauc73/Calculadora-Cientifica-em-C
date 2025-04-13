#ifndef _HISTORIC_H    
#define _HISTORIC_H
#include "solve_equation.h"

#define HISTORIC_FILE_NAME "historico.bin"

int save_historic(MathExpression *exp);
int get_historic(MathExpression *exp);
#endif