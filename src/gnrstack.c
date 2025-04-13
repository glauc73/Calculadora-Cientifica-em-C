#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gnrstack.h"

void alocation_failure(void* ptr) {
	if(ptr == NULL){
		printf("falha ao alocar memoria\n");
		exit(EXIT_FAILURE);
	}
}

stack_t* CreateStack(int capacity, DataType type) {
	if(capacity < 0) {
		printf("a capacidade da pilha deve ser um numero inteiro nao negativo\n");
		return NULL;
	}
	stack_t* stk = malloc(sizeof(stack_t));
	stk->type = type;
	switch (stk->type)
	{
	case INT_TYPE:
		stk->data.int_p =(int*) malloc(capacity* sizeof(int));
		alocation_failure(stk->data.int_p);
		break;
	case UINT_TYPE:
		stk->data.uint_p = (unsigned *) malloc(capacity* sizeof(unsigned));
		alocation_failure(stk->data.uint_p);
		break;
	case FLOAT_TYPE:
		stk->data.float_p =(float*) malloc(capacity* sizeof(float));
		alocation_failure(stk->data.float_p);
		break;
	case DOUBLE_TYPE:
		stk->data.double_p =(double*) malloc(capacity* sizeof(double));
		alocation_failure(stk->data.double_p);
		break;
	case CHAR_TYPE:
		stk->data.char_p =(char*) malloc(capacity* sizeof(char));
		alocation_failure(stk->data.char_p);
		break;
	default:
		printf("%d não representa nenhum tipo primitivo", stk->type);
		free(stk);
		return NULL;
	}
	stk->top = -1;
	stk->capacity= capacity;
	return stk;
}
void DestroyStack(stack_t* stack) {
	if(stack == NULL) {
		printf("erro: o ponteiro para a pilha é nulo\n");
		return;
	}
	switch (stack->type)
	{
	case INT_TYPE:
		free(stack->data.int_p);
		stack->data.int_p = NULL;
		break;
	case UINT_TYPE:
		free(stack->data.uint_p);
		stack->data.uint_p = NULL;
		break;
	case FLOAT_TYPE:
		free(stack->data.float_p);
		stack->data.float_p = NULL;
		break;
	case DOUBLE_TYPE:
		free(stack->data.double_p);
		stack->data.double_p = NULL;
		break;
	case CHAR_TYPE:
		free(stack->data.char_p);
		stack->data.char_p = NULL;
		break;
	}
	free(stack);
	stack = NULL;
}

void StackGrowUp(stack_t* stack) {
	switch (stack->type)
	{
	case INT_TYPE:
		stack->capacity = (stack->capacity + 1) * STACK_GROWTH;
		stack->data.int_p = realloc(stack->data.int_p, stack->capacity* sizeof(int));
		break;
	case UINT_TYPE:
		stack->capacity = (stack->capacity + 1) * STACK_GROWTH;
		stack->data.uint_p = realloc(stack->data.uint_p, stack->capacity * sizeof(unsigned));
		break;
	case FLOAT_TYPE:
		stack->capacity = (stack->capacity + 1) * STACK_GROWTH;
		stack->data.float_p = realloc(stack->data.float_p, stack->capacity * sizeof(float));
		break;
	case DOUBLE_TYPE:
		stack->capacity = (stack->capacity + 1) * STACK_GROWTH;
		stack->data.double_p = realloc(stack->data.double_p, stack->capacity * sizeof(double));
		break;
	case CHAR_TYPE:
		stack->capacity = (stack->capacity + 1) * STACK_GROWTH;
		stack->data.char_p = realloc(stack->data.char_p, stack->capacity * sizeof(char));
		break;
	}
}

void StackPush(stack_t* stack, void* value) {
	if(stack == NULL || value == NULL) {
		printf("erro: o ponteiro para a pilha ou para o valor a empilhar é nulo\n");
		return;
	}
	if(stack->top >= stack->capacity - 1) {
		StackGrowUp(stack);
	}
	
	switch (stack->type)
	{
	case INT_TYPE:
		stack->data.int_p[++stack->top] = *(int*)value;
		break;
	case UINT_TYPE:
		stack->data.uint_p[++stack->top] = *(unsigned*)value;
		break;
	case FLOAT_TYPE:
		stack->data.float_p[++stack->top] = *(float*)value;
		break;
	case DOUBLE_TYPE:
		stack->data.double_p[++stack->top] = *(double*)value;
		break;
	case CHAR_TYPE:
		stack->data.char_p[++stack->top] = *(char*)value;
		break;
	}
}
int empty(stack_t* stack) {
	if(stack == NULL) {
		printf("erro: o ponteiro para a pilha é nulo\n");
		return -1;
	}
	return stack->top == -1;
}

void StackPop(stack_t* stack, void* get) {
	if(stack == NULL || get == NULL) {
		printf("erro: o ponteiro para a pilha ou para o valor a desempilhar é nulo\n");
		return;
	}
	if(empty(stack)) {
		printf("empty stack\n");
		exit(EXIT_FAILURE);
	}

	switch (stack->type)
	{
	case INT_TYPE:
		*(int*)get = stack->data.int_p[stack->top--];
		break;
	case UINT_TYPE:
		*(unsigned*)get= stack->data.uint_p[stack->top--];
		break;
	case FLOAT_TYPE:
		*(float*)get = stack->data.float_p[stack->top--];
		break;
	case DOUBLE_TYPE:
		*(double*)get = stack->data.double_p[stack->top--];
		break;
	case CHAR_TYPE:
		*(char*)get = stack->data.char_p[stack->top--];
		break;
	}
}

void StackPeek(stack_t* stack, void* peek) {
	if(stack == NULL || peek == NULL) {
		printf("erro: o ponteiro para a pilha ou para o valor do topo é nulo\n");
		return;
	}

	switch (stack->type)
	{
	case INT_TYPE:
		*(int*)peek = stack->data.int_p[stack->top];
		break;
	case UINT_TYPE:
		*(unsigned*)peek= stack->data.uint_p[stack->top];
		break;
	case FLOAT_TYPE:
		*(float*)peek = stack->data.float_p[stack->top];
		break;
	case DOUBLE_TYPE:
		*(double*)peek = stack->data.double_p[stack->top];
		break;
	case CHAR_TYPE:
		*(char*)peek = stack->data.char_p[stack->top];
		break;
	}
}

int balanced_brackets(char* exp) {
	stack_t* stk = CreateStack(Stack_BUFF, CHAR_TYPE);
	char get_top;
	int count = 0;
	if(stk == NULL) {
		printf("memoria insuficiente");
		exit(EXIT_FAILURE);
	}

	while(*exp) {
		if(*exp == '('){  //se encontrar uma abertura
			StackPush(stk, exp);
		}else if(*exp == ')'){ //se encontrar um fechamento, remove o que está no topo da pilha e compara
			if(empty(stk)) {
				DestroyStack(stk);
				return 0;
			}
			StackPop(stk, &get_top);
		}
		exp++;
	}
	int result = empty(stk);
	DestroyStack(stk);
	return result;
}
