#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "math_parser.h"

signed char var = 'x';
angle_type angle_parser = RAD;

const func_map functions[] = // mapa contendo as funcoes e suas respectivas strings
{ 
	{"tan(", parse_tan},
	{"sen(", parse_sen},
	{"cos(", parse_cos},
	{"arctan(", parse_arctan},
	{"arcsen(", parse_arcsen},
	{"arccos(", parse_arccos},
	{"log(", parse_log},
	{"ln(", parse_ln},
	{"sqrt(", parse_sqrt},
	{"cbrt(", parse_cbrt},
	{"abs(", parse_abs},
	{"e", euller_number},
	{"pi", PI_}, 
	{"(", parse_bracket},
};

void strlower(char *str)
{
	while (*str)
	{
		if (*str >= 'A' && *str <= 'Z')
		{
			*str += 'a' - 'A';  //soma a diferença entre maiusculo e minusculo na tabela ascii, convertendo
		}
		str++;
	}
}
double angle_mode()
{
	return (angle_parser == RAD ? 1 : M_PI / 180);  //se a variavel de angulo for RAD, retorna 1, se for DEGREE, retorna o fator de conversão pra radiano
}

//verifica se o ponteiro ata=ual é uma função definida e retorna seu tamanho
//retorna 0 se não for uma função
int lenFunc(char *exp)
{
	for (size_t i = 0; i < SIZE_ARR(functions); i++)
	{
		if (strncmp(exp, functions[i].name, strlen(functions[i].name)) == 0)  //compara se a posição atual do ponteiro tem o nome de uma função
		{
			return strlen(functions[i].name); //se tiver, retorna o tamanho da função
		}
	}
	return 0;
}

//é a ultima chamada da função, e retorna o numero na qual o ponteiro aponta
double parse_number(char **exp){
	char *endptr;
	double result = strtod(*exp, &endptr);
	*exp = endptr;   //a expressão recebe o ponteiro para o final do numero, encerrando a função/ recursão e começando a retornar
	return result;
}

double parse_bracket(char **exp) {
	*exp += strlen("(");
	//ao encontrar um parenteses, a função chama a si mesma de forma recursiva, pois a expressão dentro dele independe do resto.
	double result = parseexp(exp); 
	//esse raciocinio se segue para as demais funções
	(*exp)++;
	return result;
}

double parse_abs(char **exp) {
	*exp += strlen("abs(");
	double result = fabs(parseexp(exp));
	(*exp)++;
	return result;
}

double parse_sqrt(char **exp) {
	*exp += strlen("sqrt(");
	double result = sqrt(parseexp(exp));
	(*exp)++;
	return result;
}

double parse_cbrt(char **exp) {
	*exp += strlen("cbrt(");
	double result = cbrt(parseexp(exp));
	(*exp)++;
	return result;
}

double parse_tan(char **exp)
{
	*exp += strlen("tan(");
	double result = tan(parseexp(exp) * angle_mode());
	(*exp)++;
	return result;
}

double parse_sen(char **exp)
{
	*exp += strlen("sen(");
	double result = sin(parseexp(exp) * angle_mode());
	(*exp)++;
	return result;
}

double parse_cos(char **exp)
{
	*exp += strlen("cos(");
	double result = cos(parseexp(exp) * angle_mode());
	(*exp)++;
	return result;
}

double parse_arcsen(char **exp) {
	*exp += strlen("arcsen(");
	double result = asin(parseexp(exp));
	(*exp)++;
	return result;
}

double parse_arccos(char **exp) {
	*exp += strlen("arccos(");
	double result = acos(parseexp(exp));
	(*exp)++;
	return result;
}

double parse_arctan(char **exp) {
	*exp += strlen("arctan(");
	double result = atan(parseexp(exp));
	(*exp)++;
	return result;
}

double parse_log(char **exp)
{
	*exp += strlen("log(");
	double result = log10(parseexp(exp));
	(*exp)++;
	return result;
}

double parse_ln(char **exp)
{
	*exp += strlen("ln(");
	double result = log(parseexp(exp));
	(*exp)++;
	return result;
}

double euller_number(char **exp)
{
	*exp += strlen("e");
	return M_e;
}

double PI_(char **exp)
{
	*exp += strlen("pi");
	return M_PI;
}

double parsepow(char **exp){
	// checa se a posicao atual do ponteiro é uma das funcoes mapeadas (sen, cos, log, etc)
	for (size_t i = 0; i < sizeof(functions) / sizeof(func_map); i++)
	{
		if (strncmp(*exp, functions[i].name, strlen(functions[i].name)) == 0)
		{
			return functions[i].call(exp);  //se for alguma das funções, ele a retorna;
		}
	}
	// default : recebe o numero
	return parse_number(exp);
}

double parsefactor(char **exp){
	double factor = parsepow(exp);
	double result = factor;
	while (**exp == '^')
	{
		while (**exp == '^')  //skipa no caso de o usuario digitar '^^^^^^'
			(*exp)++;
		
		double nextfactor = parsepow(exp);
		result = pow(result, nextfactor);	
	}
	return result;
}

double parseterm(char **exp){
	double factor = parsefactor(exp);
	double result = factor;
	while (**exp == '*' || **exp == '/')
	{
		char op = **exp;
		while(**exp == '*' || **exp == '/') {
			(*exp)++;
		}
		 // lembre-se de incrementar o ponteiro para nao analisar de novo o sinal e dar erro no retorno
		double nextfactor = parsefactor(exp);
		result *= (op == '*') ? nextfactor : 1 / nextfactor;
	}
	return result;
}

double parseexp(char **exp){

	double term = parseterm(exp);
	double result = term;
	while (**exp == '+' || **exp == '-')
	{
		char op = **exp;
		while (**exp == '+' || **exp == '-'){
			(*exp)++;
		}
		
		double nextterm = parseterm(exp);
		result += (op == '+') ? nextterm : -nextterm;
	}
	
	return result;
}

double eval(char *exp)
{
	strlower(exp);
	return parseexp(&exp);
}

double eval_X(char *exp, double X)
{
	strlower(exp);
	// array que vai guardar a nova funcao com o conteudo de X
	char* func_x = calloc(10 * strlen(exp), sizeof(char));
	while (*exp)
	{
		int len_func = lenFunc(exp);

		if(len_func) {  //verifica as funções antes, porque o usuartio pode escolher uma variavel que a compoe. ex: s*sen(pi/6) = 2
			strncat(func_x, exp, len_func);
			exp += len_func;
			continue;
		}
		if (*exp == var){
			char value[32];
			sprintf(value, "(%.6lf)", X);
			strcat(func_x, value);  ///coloca o numero entre parenteses na expressão, para respeitar a ordem de precedencia
		}else {
			strncat(func_x, exp, 1);
		}
		exp++;
	}
	char *ptr = func_x;
	double result = parseexp(&ptr);
	free(func_x);
	return result;
}

bool eval_bool(char* exp){
	
	char *left_side = exp;
	char *right_side = strchr(exp, '=');
	
	if (right_side != strrchr(exp, '=')){
		printf("formato invalido para equacoes booleanas!\n");
		return 0;
	}
	*right_side++ = '\0';
	double right_value = eval(right_side);
	double left_value = eval(left_side);
	
	return (fabs(left_value - right_value) < dx);
}

