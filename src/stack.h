#ifndef STACK_H
#define STACK_H
#include "types.h"
#define STACK_SIZE 1000

void push(obj_t *);
obj_t *pop(void);
void drop(void);
obj_t *stack_obj(int);
#endif
