#ifndef TYPES_H
#define TYPES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
typedef enum {false,true} bool;
typedef enum {
	CELL,
	DOUBLE,
	ERROR,
	HASHTABLE,
	INTEGER,
	FUNCTION,
	SYMBOL,
} type_t;
extern const char *type_names[];
typedef struct obj_s {
	int refs;
	type_t type;
	union {
		struct {
			struct obj_s *car;
			struct obj_s *cdr;
		} cell;
		struct {
			bool lambda;
			union {
				void (*c)(void);
				struct obj_s **lisp;
			} rep;
		} func;
		char *sym;
		long i;
		double d;
		table_t *table;
	} data;
} obj_t;
#define CAR(x) ((x)->data.cell.car)
#define CDR(x) ((x)->data.cell.cdr)

obj_t *new_obj(void);
obj_t *new_cell(obj_t *,obj_t *);
obj_t *new_symbol(char *);
obj_t *new_integer(long);
obj_t *new_double(double);
obj_t *new_hashtable(int);
obj_t *new_cfunction(void (*)(void));
void destroy(obj_t *);
obj_t *incr_refs(obj_t *);
void decr_refs(obj_t *);
void print_obj(obj_t *);
bool eq_objs(obj_t *,obj_t *);
#endif
