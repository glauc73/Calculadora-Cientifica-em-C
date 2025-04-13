#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "gnrstack.h"
#include "solve_equation.h"
#include "config.h"

const char valid_operate[] = {'+', '-', '*', '/', '(', ')', '^', '='};
extern program_config settings;

//verifica se é um dos operadores validos
bool isoperat(char ch)
{
	for (size_t i = 0; i < SIZE_ARR(valid_operate); i++)
	{
		if(ch == valid_operate[i])   //procura o caracter mencionado no array de operadores validos
			return 1;
	}
	return 0;
}

void removespace(char* exp) {
	char temp[strlen(exp) + 1];
	char *ptr = temp;
	char* begin_exp = exp;
	while(*exp) {
		if(!isspace(*exp)) {
			*ptr++ = *exp;
		}
		exp++;
	}
	*ptr = '\0';
	strcpy(begin_exp, temp);
}

//função de ordenação crescente para qsort
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

//retorna: 0, caso não tenha caractere invalido. caso haja, retorna esse caractere
char invalid_char(char *iterator)
{
	while (*iterator)
	{
		if (ispunct(*iterator) && !isoperat(*iterator) ){ //verifica se possui um operador invalido (simbolo que nao está em valid operate)
			return *iterator;  //retorna esse caracter se encontrar
		}
		iterator++;
	}
	return 0;
}

/*verifica se exp possui variavel literal
retorna: caso exista uma: a variável.
se houver mais de uma: a constante MULTIPLY_VAR
se não houver: a constante NOT_VAR*/
char checkvar(char *exp)
{
	int count = 0;
	char varx = 0, atual_var = 0;
	
	while (*exp)
	{	int len_func = lenFunc(exp);
		if(len_func) {  //ignora os caracteres das funções, para não confundir com variaveis
			exp +=len_func;
			continue;
		}
		if(!isalpha(*exp)) { //se não for uma letra, ignora e retoma o loop
			exp++;
			continue;
		}
		atual_var = *exp;

		if(count == 0) {
			varx = atual_var;
		}else if(atual_var != varx){
			return MULTIPLY_VAR;
		}
		count++;
		//a cada caracter novo encontrado, adiciona no array e compara com var[0]
		exp++; //caso o ponteiro atual aponte uma função, exp a pula, se não, pula penas 1;
	}

	return count == 0 ? NOT_VAR : varx;
}

// copia o conteudo de input para func, adicionando '*' nas multiplicações implicitas;
void add_product(char *input)
{	
	char* func = calloc(6 * strlen(input), sizeof(char));
	char* aux = input;
	while (*input)
	{	
		int len_func = lenFunc(input);

		if(len_func){  //antes de tudo verifica se o ponteiro aponta uma função, para copiá- la de uma vez. permitindo que xln(x) vire x*ln(x), por exemplo
			strncat(func, input, len_func);
			input += len_func;
			continue;
		}
		strncat(func, input, 1); //copia caracter a caracter input para func

		char atual = *input;
		char next = *(input + 1);
		//nos seguintes casos o usuario usou uma multiplicação implicita. o programa adiciona '*' entre os caracteres para que eval() possa interpretar.
		
		int implicit_product = 
			(isdigit(atual) && isalpha(next)) 	|| 	// 2x -> 2*x
			(isalpha(atual) && isdigit(next))	||	// x2 -> x*2
			((atual == ')' && next == '(') )	||	// )( -> )*(
			(isalpha(atual) && isalpha(next)) 	||	// xy -> x*y
			(isalpha(atual) && next == '(') 	|| 	// x( -> x*(
			(atual == ')' && isalpha(next));		// )x -> )*x

		if (implicit_product){
			strcat(func, "*");
		}
		input++;
	}
	strcpy(aux, func); 
	free(func); //copia func para o ponteiro inicial de input, que está em aux 
}

//formata uma equação, colocando tudo de um lado só, respeitando a troca de sinais
void Create_Function(MathExpression* exp)
{
	char func[2 * strlen(exp->input) + 1];
	for(size_t i = 0; i < strlen(exp->input) + 1; i++) {
		func[i] = 0;
	}
	strcpy(func, exp->side.left);
	
	// caso nao tenha sinal de igual, a funcao esta formatada enquanto aos lados
	if (exp->side.right == NULL || exp->side.left == NULL)
	return;
	// caso o primeiro elemento do lado direito apos passar espacos nao seja sinal, coloca um sinal de '-' no lado esquerdo
	if (*exp->side.right != '+' && *exp->side.right != '-'){
		strcat(func, "-");
	}
	
	while (*exp->side.right)
	{
		
		switch (*exp->side.right)
		{
			case '+':
			strcat(func, "-");
			break;
			case '-':
			strcat(func, "+");
			break;
			case ' ':
			break;
			default:
			strncat(func, exp->side.right, 1);
			break;
		}
		exp->side.right++;
	}
	strcpy(exp->input, func); //copia a função formatada para exp->input
	return;
}

//verifica se a expressão é uma identidade (sempre verdadeira)
int isIdentity(MathExpression* exp) {
	if(exp->side.right == NULL) {
		return 0;
	}
	if(strcmp(exp->side.left, exp->side.right) == 0) {
		exp->status = BOOLE;   //se os lados forem exatamente iguais, a expressão é sempre verdadeira.
		return 1;
	}
	//algumas vezes a expressão não é exatamente igual, ex: x = 1 + x - 1, para tentar melhorar isso, testarei todos os valores da metade do range em eval_x, e 
	//se em algum deles a diferença dos evals dos dois lados não for zero, então a expressão nao pode ser uma igualdade
	double diff_sides[settings.range + 1];
	for(int i = -settings.range/2; i <= (int)settings.range/2; i++ ) { 
		diff_sides[i + settings.range/2] = fabs(eval_X(exp->side.left, i) - eval_X(exp->side.right, i));  
		if(diff_sides[i + settings.range/2] >= dx || isnan(diff_sides[i + settings.range/2]) ) {
			return 0;
		}
	}
	return 1;
}

/*Recebe: ponteiro para expressão matematica
retorna: 
se exp->status = BOOLE, 1 ou 0
se exp->status = EVAL, o resultado da expresão
se exp->status = EQUAION, a quantidade de raízes adicionadas a exp->root
*/
double solve(MathExpression* exp){
	exp->side.left = exp->input;  //ponteiro pro lado esquerdo
	exp->side.right = strchr(exp->input, '=');  //procura a primeira ocorrencia do caracter '=', pois a partir do próximo endereço será o lado direito
	exp->var = checkvar(exp->input);  //verifica se a expressão tem uma, mais de uma ou nenhuma variavel literal
	var = exp->var;
	char ch = invalid_char(exp->input); //verifica se tem algum caracter inválido, com	o '&', '?' e etc
	
	if(ch != 0) exp->status = INVALID_CHAR;
	else if(exp->side.left == exp->side.right)	exp->status = NO_LEFT;
	else if(!balanced_brackets(exp->input)) 	exp->status = BRACKETS_ODD; 
	else if (exp->side.right == NULL && !exp->var) 	exp->status = EVAL;
	else if(exp->var == MULTIPLY_VAR) 	exp->status = MULTIPLY_VAR;
	else if (exp->side.right == NULL)	exp->status = FORMAT_ERROR;
	else if(exp->side.right != strrchr(exp->input, '=')) 	exp->status = FORMAT_ERROR;
	else if(!exp->var && exp->side.right) 	exp->status = BOOLE;
	else exp->status = EQUATION; //se nenhuma das outras excessões ocorrer, então a expressão é uma expressão bem formada a ser resolvida
	
	char *equal_sign = exp->side.right; // passa o endereço do sinal de igual para equal_sign, pois ele será alterado e depois
	// precisará ser restaurado
	
	if(exp->side.right && exp->status == EQUATION)	{ //se exp->side.right == 	NULL, e tentarmos referenciar o ponteiro exp->side.right, ocorre segfault, logo devemos fazer essa verificação
		*exp->side.right++ = '\0';	  // atribui '\0' ao sinal de igual para dividir os lados e avança o ponteiro para o lado direito
		if (*(exp->side.right) == '\0'){
			exp->status = NO_RIGHT;// se após os espaços houver um caracter nulo, significa que o lado direito está vazio{ 
		}
		if(isIdentity(exp)){
			exp->status = BOOLE;
			return 1;
		}
	}

	switch (exp->status)
	{
	case NO_LEFT:
		printf("Erro: lado esquerdo vazio\n");
		exp->status = NO_LEFT;
		return NAN;
	case NO_RIGHT:
		printf("Erro: lado direito vazio\n");
		return NAN;
	case EVAL:
		return eval(exp->input);
	case FORMAT_ERROR:
		printf("Erro: formato de equacao invalido: equacoes possuem exatamente um sinal de igual\n");
		return NAN;
	case BOOLE:
		int logic = eval_bool(exp->input);
		return logic;
	case MULTIPLY_VAR:
		printf("Erro: expressao com muitas variaveis\n");
		return NAN;
	case INVALID_CHAR:
		printf("Erro: caracter invalido : %c", ch);
		return NAN;
	case BRACKETS_ODD:
		printf("Erro: Parenteses desbalanceados. Verifique se cada '(' tem um ')' correspondente.\n");
		return NAN;
	case EQUATION:	
		Create_Function(exp);
		
		exp->count_root = select_root(exp->input, exp);  //seleciona apenas as raízes distintas no array root.
		*equal_sign = '=';	//devolve o sinal de igual para o lugar em que estava
		
		return exp->count_root;
	}
}

//le uma expressão aritmética e retorna seu resultado
double get_eval(const char* msg) {
	char rbuf[BUFSIZE];
	while(1) {
		printf(msg);
		solve_type read_status = EVAL;
		fgets(rbuf, BUFSIZE, stdin);

		rbuf[strcspn(rbuf, "\n")] = '\0';
		char ch = invalid_char(rbuf);
	
		if(checkvar(rbuf) != NOT_VAR) read_status = EVAL_X;  
		if(!balanced_brackets(rbuf)) read_status = BRACKETS_ODD;
		if(ch != 0) read_status = INVALID_CHAR;
		if(rbuf[0] == '\0') read_status = NO_RIGHT; 

		switch (read_status)
		{
		case EVAL:  //informa que a read_status nao foi alterada
			removespace(rbuf);
			strlower(rbuf);
			add_product(rbuf);  //formata o eval para não demonstrar comportamento indefinido e em seguida retorna
			return eval(rbuf);
		case EVAL_X:
			printf("informe uma expressao numerica sem incognita\n");
			break;
		case BRACKETS_ODD:
			printf("Erro: Parenteses desbalanceados. Verifique se cada '(' tem um ')' correspondente.\n");
			break;
		case INVALID_CHAR:
			printf("Erro: caracter invalido : %c\n", ch);
			break;
		case NO_RIGHT:
			printf("informe uma expressao numerica\n");
			break;
		default:
			printf("erro desconhecido\n");
			break;
		}
	}
}