#include <stdio.h>
#include <stdlib.h>
#include "solve_equation.h"
#include "historic.h"

int save_historic(MathExpression *exp)
{
	FILE *file = fopen(HISTORIC_FILE_NAME, "ab");
	if(file == NULL) {
		printf("falha ao abrir o arquivo");
		exit(1);
	}
	fwrite(exp, sizeof(MathExpression), 1, file);
	fclose(file);
	return 1;
}

int get_historic(MathExpression *exp)
{
	FILE *file = fopen(HISTORIC_FILE_NAME, "rb");

	//lê cada expressão e armazena em exp. o comportamento depende da variavel de status
	while(fread(exp, sizeof(MathExpression), 1, file)){

		switch (exp->status)
		{
		case EVAL:
			printf("%s = %lf\n", exp->input, exp->result);
			break;
		case EQUATION:
			if(exp->count_root == 0) {
				printf("%s nao possui raizes reais\n", exp->input);
			}
			printf("raizes reais de %s :\n", exp->input);
			for(int i = 0; i < exp->count_root; i++) {
				printf("%c%d = %lf\n", exp->var, i + 1, exp->root[i]);
			}
			break;
		case BOOLE:
			printf("%s\n%s\n", exp->input,  exp->result ? "verdadeiro" : "falso");
			break;
		case EVAL_X:
			double x;
			fread(&x, sizeof(double), 1, file);
			printf("f(%c) = %s\n ", exp->var, exp->input);
			printf("f(%lf) = %lf\n", x, eval_X(exp->input, x));
			break;
		default:
			printf("erro inesperado\n");  //uma forma de depurar futuros erros que possam aparecer, mesmo que na pratica exp->status não vá ser nenhum outro valor
			exit(EXIT_SUCCESS);
		}
		putchar('\n');
		printf("--------------------------------------------------\n");
	}
	fclose(file);
	return 1;
}
