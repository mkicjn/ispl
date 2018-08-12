#ifndef NAMESPACE_H
#define NAMESPACE_H
#include "core.h"
#include "types.h"

void push_namespace(table_t *);
table_t *pop_namespace(void);
void drop_namespace(void);
table_t *new_namespace(void);
obj_t *get_binding(obj_t *);
void set_binding(obj_t *,obj_t *);
void unset_binding(obj_t *);
#endif