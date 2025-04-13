#ifndef _GNRSTACK_H
#define _GNRSTACK_H

#define Stack_BUFF 128
#define STACK_GROWTH 1.5

typedef enum{
	INT_TYPE = 3,
	UINT_TYPE,
	FLOAT_TYPE, 
	DOUBLE_TYPE, 
	CHAR_TYPE
}DataType;

union generic_ptr{
	char *char_p;
	int *int_p;
	unsigned *uint_p;
	float *float_p;
	double *double_p;
};

typedef struct{
	union generic_ptr data;
	int top;
	int capacity;
	DataType type; 
}stack_t;

void alocation_failure(void* ptr);
stack_t* CreateStack(int capacity, DataType type);
void DestroyStack(stack_t* stack);
void StackGrowUp(stack_t* stack);
void StackPush(stack_t* stack, void* value);
int empty(stack_t* stack);
void StackPop(stack_t* stack, void* get);
void StackPeek(stack_t* stack, void* peek);
int balanced_brackets(char* exp);

#endif