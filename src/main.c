#include <math.h>
#include "safeget.h"
#include "gnrstack.h"
#include "math_parser.h"
#include "solve_equation.h"
#include "config.h"
#include "historic.h"
#include "rootscan.h"

void call_solve(MathExpression* exp);
void call_eval_X(MathExpression* exp, double x);
void map_operations();
void table_functions();

extern char valid_operate[NUM_VALID_OPERATE];
extern func_map functions[NUM_FUNCTIONS];
extern program_config settings;
extern signed char var;

int main()
{
	get_config_file();
	MathExpression exp = {.status = EVAL};  //inicia com o formato padrão
	uint8_t op;
	
	map_operations();
	do{
		exp.count_root = 0;
		printf
		(
			" angulo : %s\t\t  "
			"intervalo: [-%d, %d]\n",
			settings.angle == RAD ? "RAD" : "GRAU",
			settings.range, settings.range
		);
		
		printf
		(	
			"--------------------------------------------------\n"
			"             CALCULADORA CIENTIFICA:\n"
			"--------------------------------------------------\n"
			"1) Resolver expressao numerica, booleana ou equacao\n"
			"--------------------------------------------------\n"
			"2) Encontrar valor da funcao no ponto\n"
			"--------------------------------------------------\n"
			"3) Consultar historico\n"
			"--------------------------------------------------\n"
			"4) Apagar historico\n"
			"--------------------------------------------------\n"
			"5) Lista de funcoes validas\n"
			"--------------------------------------------------\n"
			"6) Configuracoes\n"
			"--------------------------------------------------\n"
			"0) Sair\n"
			"--------------------------------------------------\n"
		);
		op = get_u64_limited(0, 6, "informe uma opcao valida: ");
		
		switch (op)
		{
		case 1:
			system("cls");
			printf
			(
				"--------------------------------------------------\n"
				"informe uma expressao:\n"
			);
			scanf("%512[^\n]s", exp.input);
			setbuf(stdin, NULL);
			
			call_solve(&exp);

			printf("--------------------------------------------------\n");
			break;
		case 2:
			system("cls");
			exp.status = EVAL_X;
			printf
			(
				"--------------------------------------------------\n"
				"informe uma expressao:\n"
				"f(x) = "
			);
			scanf("%512[^\n]s", exp.input);
			setbuf(stdin, NULL);
			
			double x0 = get_eval("x: "); //usado para ler uma expressão aritmetica que retorna seu resultado em double ao inves de ler apenas um simples numero
			call_eval_X(&exp, x0);
			printf("--------------------------------------------------\n");
			break;
		case 3:
			system("cls");
			printf
			(
				"--------------------------------------------------\n"
				"                    HISTORICO\n"
				"--------------------------------------------------\n"
			);
			get_historic(&exp);
			break;
		case 4:
			system("cls");
			printf
			(
				remove(HISTORIC_FILE_NAME) ? 
				"historico vazio\n\n" : "historico apagado com sucesso\n\n"
			);
			break;
		case 5:
			system("cls");
			table_functions();
			break;
		case 6:
			system("cls");
			printf 
			(	
				"--------------------------------------------------\n"
				"                  CONFIGURACOES\n"
				"--------------------------------------------------\n"
				"1) selecionar unidade de medida de angulo\n"
				"--------------------------------------------------\n"
				"2) selecionar intervalo de candidatos a x0 [-a, a]\n"
				"--------------------------------------------------\n"
				"0) sair\n"
			);
			int choice = get_i64_limited(0, 2, "informe uma opcao valida: ");
			select_config(choice);
			break;
		default:
			break;
		}
	}while(op);
	return 0;
}

void call_solve(MathExpression* exp)
{
	strlower(exp->input);
	char output[strlen(exp->input) + 1]; // expressão que guarda a expressão para saída, pois o ponteiro será alterado na resolução
	strcpy(output, exp->input);

	removespace(exp->input); //remove os espaçõs para evitar comportamento indefinido
	add_product(exp->input);

	exp->result = solve(exp);

	strcpy(exp->input, output); //retorna a string original para o campo input após atribuir as soluções em exp
	
	//mostra ao usuario de acordo com exp->status
	switch (exp->status)
	{
	case EVAL:
		printf("%s = %lf\n", output, exp->result);
		break;
	case EQUATION:
		if(exp->count_root == 0) {
			printf("%s nao possui raizes reais\n", exp->input);
			return;
		}
		printf("raizes reais de %s :\n", exp->input);
		for(int i = 0; i < exp->count_root; i++) {
			printf("%c%d = %lf\n", exp->var, i + 1, exp->root[i]);
		}
		break;
	case BOOLE:
		printf("%s\n", exp->result ? "verdadeiro" : "falso");
		break;
	default:
		return;
	}
	save_historic(exp);
}
void call_eval_X(MathExpression* exp, double x){
	removespace(exp->input);
	strlower(exp->input);  //formata a função eliminando espaços, maiusculas e adicionando o caracter '*'
	add_product(exp->input);  
	
	var = checkvar(exp->input);
	exp->var = var;  //atribui a variavel da expresão
	char ch = invalid_char(exp->input);
	
	if(exp->input[0] == '\0') exp->status = NO_RIGHT;
	else if(ch != 0) exp->status = INVALID_CHAR;
	else if(strchr(exp->input, '=')) exp->status = FORMAT_ERROR;
	else if(!balanced_brackets(exp->input)) exp->status = BRACKETS_ODD;
	else if(var == MULTIPLY_VAR) exp->status = MULTIPLY_VAR;
	
	switch (exp->status)
	{
	case NO_RIGHT:
		printf("Erro: espera-se uma expressao\n");
		break;
	case INVALID_CHAR:
		printf("Erro: caractere invalido: '%c'\n", ch);
		break;
	case FORMAT_ERROR:
		printf("Erro: funcoes possuem um, e somente um sinal de igual\n");
		break;
	case BRACKETS_ODD:
		printf("Erro: Parenteses desbalanceados. cada '(' deve ter um ')' correspondente.\n");
		break;
	case MULTIPLY_VAR:
		printf("Erro: equacao com muitas variaveis\n");
		break;
	default:
		printf("f(%lf) = %lf\n", x, eval_X(exp->input, x));
		save_historic(exp);

		FILE *file = fopen(HISTORIC_FILE_NAME, "ab");
		fwrite(&x, sizeof(double), 1, file);   //guarda o valor de X no arquivo para que possa ser incluido em eval_x no historico.
		fclose(file);
		break;
	}
}

void map_operations() {
	printf("operadores suportados: \n");
	for(int i = 0; i < NUM_VALID_OPERATE; i++){
		printf("[ %c ]  ", valid_operate[i]);
		if(i % 4 == 3) putchar('\n');
	}
	printf("\nfuncoes suportadas: \n");
	for(int i = 0; i < NUM_FUNCTIONS - 3; i++){
		printf("[ %-7s ]  ", functions[i].name);
		if(i % 4 == 3) putchar('\n');
	}
	printf("\n\nconstantes suportadas: \n");
	printf("[ e ] [ pi ]\n");
	putchar('\n');
}

void table_functions() {
	printf
	(
		"[    e    ] -> numero de euller\n"
		"[   sen(  ] -> seno\n"
		"[   cos(  ] -> cosseno\n"
		"[   tan(  ] -> tangente\n"
		"[ arcsen( ] -> arco-seno\n"
		"[ arccos( ] -> arco-cosseno\n"
		"[ arctan( ] -> arco-tangente\n"
		"[  sqrt(  ] -> raiz quadrada\n"
		"[  cbrt(  ] -> raiz cubica\n"
		"[   ln(   ] -> logaritmo na base e\n"
		"[  log(   ] -> logaritmo na base 10\n"
		"[  abs(   ] -> modulo (valor absoluto)\n"
	);
	putchar('\n');
}
