#include "solve_equation.h"
#include "rootscan.h"

extern program_config settings;

int comp(const void *a, const void *b)
{
	if (*(double *)a < *(double *)b)
		return -1;
	else if (*(double *)a > *(double *)b)
		return 1;
	return 0;
}

double derivate(char *exp, double value){
	int temp = settings.angle;
	settings.angle = RAD;
	return ((eval_X(exp, (value + dx)) - eval_X(exp, value) )/ dx); //retorna a derivada de exo
	settings.angle = temp;
}

size_t select_root(char *func, MathExpression* exp)
{
	int range_size = (2 * settings.range + 1 ) / JUMP_X0; 
	double test[range_size];
	
	double p = -settings.range;
	for (int i = 0; i < range_size ; i++){
		test[i] = p;//nesse intervalo diminui a passada, para que equações envolvendo arco seno, por exenplo, tenham solução
		p += (-1 <= p && p <= 1) ? 0.2 : JUMP_X0;
	}
	
	// adiciona cada raiz proveniente do metodo de newton
	for (int i = 0; i < range_size; i++)
		test[i] = newton_method(func, test[i]);
	
	// ordena as raizes para armazenar apenas as distintas no array "root"
	qsort(test, range_size, sizeof(double), comp);
	int count = filter_root(exp, test, range_size);
	
	return count;
}

int filter_root(MathExpression* exp,  double *root_newton, size_t range_size)
{
	size_t i = 0, count = 0;
	while(isnan(root_newton[i])){
		i++;
		if(i == range_size) return 0;  //se i percorreu todas as posicoes de root_newton entao nao existem raizes reais
	} 
	exp->root[count] = (fabs(root_newton[i]) < dx) ? 0 : root_newton[i];  //exp->root recebe o primeiro elemento após os nan, que será nossa referencia para comparação
	count++;
	while (i < range_size)
	{
		if(!DBLCMP(root_newton[i], exp->root[count - 1]) && !isnan(root_newton[i])){		
			exp->root[count++] = root_newton[i]; 
		}
		// caso o conteudo do array seja NAN, significa que um erro ocorreu
		i++;
	}
	return count;
}

double newton_method(char *exp, double x0)
{
	double x = x0;
	int i;
	for (i = 0; i < NEWTON_ITERATIONS ; i++)
	{
		double f_x = eval_X(exp, x);
		if(isnan(f_x) ) return NAN;
		
		double f_x_derivate = derivate(exp, x);
		if (fabs(f_x_derivate) < EPSILON) {	//indica que incontrou um ponto de inflexao (derivada proxima o suficiente de 0)
			return (fabs(x - x0) < dx) && (fabs(f_x) < EPSILON)  ? x : NAN;   //caso |f_x| seja muito proximo de 0, significa que o ponto de inflexao é uma raiz, se nao, retorna NAN
		}//a condição de retorno foi, se x está convergindo e se abs(f_x) está pequeno o suficiente, pois se considerarmos apenas apenas a segunda, ao método de newton pega raízes maiores que esperado.
		//ex: x^5 = 0 pode ter 0.0042 como raiz, mesmo que isso seja mentira, pois 0.0042^5 ja da um numero muito menor
		x0 = x;
		x = x - f_x / f_x_derivate;
		
		if (fabs(x - x0) < dx) {//verifica se o valor de x está convergindo
			return (fabs(x) < dx) ? 0 : x;
		} 
	}
	return NAN; //se o metodo de newton nao convergiu a uma raiz depois de toda a iteracao, provavelmente nao existe raiz real;
}
