#include "solve_equation.h"
#include "rootscan.h"

extern program_config settings;
size_t select_root(char *func, MathExpression* exp)
{
	int range_size = (2 * settings.range + 1 ) / JUMP_X0; 
	double test[range_size];
	
	double p = -settings.range;
	for (int i = 0; i < range_size ; i++){
		//nesse intervalo diminui a passada, para que equações envolvendo arco seno, por exenplo, tenham solução
		test[i] = p;
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

int filter_root(MathExpression* exp,  double *arr, size_t range_size)
{
	size_t i = 0, count = 0;
	while(isnan(arr[i])){
		i++;
		if(i == range_size) return 0;  //se i percorreu todas as posicoes de arr entao nao existem raizes reais
	} 
	
	exp->root[count] = arr[i];  //exp->root recebe o primeiro elemento após os nan, que será nossa referencia para comparação
	count++;
	while (i < range_size)
	{
		if(!DBLCMP(arr[i], exp->root[count - 1]) && !isnan(arr[i])){
			if(fabs(arr[i]) < dx)		//na linguagem C existe o double -0. como estamos trabalhando com aproximações, é ideal prevenir sua aparição
				arr[i] = 0;				
			exp->root[count++] = arr[i]; 
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
		
		if (fabs(f_x_derivate) < EPSILON){	//indica que incontrou um ponto de inflexao (derivada proxima o suficiente de 0)
			return fabs(f_x) < dx ? x : NAN;   //caso |f_x| seja muito proximo de 0, significa que o ponto de inflexao é uma raiz, se nao, retorna NAN
		}
		x0 = x;
		x = x - f_x / f_x_derivate;
		
		if (fabs(x - x0) < dx) {//verifica se o valor de x está convergindo
			return x;
		} 
	}
	return i == NEWTON_ITERATIONS ? NAN : x; //se o metodo de newton nao convergiu a uma raiz depois de toda a iteracao, provavelmente nao existe raiz real;
}