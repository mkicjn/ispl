#include "stack.h"
static obj_t *stack[STACK_SIZE];
static int stack_index=0;
void push(obj_t *obj)
{
	incr_refs(obj);
	stack[stack_index++]=obj;
}
obj_t *pop(void)
{ // Popped stack items must have a call to decr_refs after use
	return stack[--stack_index];
}
void drop(void)
{
	decr_refs(stack[--stack_index]);
}
obj_t *stack_obj(int n)
{
	return stack[stack_index-n-1];
}
void print_stack(void)
{
	if (!stack_index) {
		fputs("Empty stack\n",stderr);
		return;
	}
	fputs("Stack:\n",stderr);
	for (int i=stack_index-1;i>=0;i--) {
		fprintf(stderr,"%d: ",i);
		print_obj(stack[i],stderr);
		fputc('\n',stderr);
	}
}
