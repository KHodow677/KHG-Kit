#pragma once

#define EXECUTE_THROW(x) longjmp(ex, (x))

typedef enum {
	op_pos,
	op_rbeg,
	op_rend
} stack_op;

typedef struct {
	stack_op op;
	const char *opr;
	short st;
} stack_el;

typedef struct {
	stack_el *els;
	short ns;
	short ts;
} stack;

stack *stack_create(int ns);
void free_stack(stack *stk);
int push(stack *stk, stack_op op, const char *opr, short state);
stack_el* pop(stack *stk);
