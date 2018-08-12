#include "core.h"
table_t *dict=NULL;
STACK(print,1_ARG)
obj_t *print(obj_t *obj)
{
	print_obj(obj);
	return obj;
}
STACK(terpri,0_ARGS)
obj_t *terpri(void)
{
	putchar('\n');
	return NULL;
}
STACK(cons,2_ARGS)
obj_t *cons(obj_t *a,obj_t *b)
{
	return new_cell(a,b);
}
STACK(car,1_ARG)
obj_t *car(obj_t *c)
{
	if (!c)
		return NULL;
	if (c->type!=CELL)
		return new_obj();
	return CAR(c);
}
STACK(cdr,1_ARG)
obj_t *cdr(obj_t *c)
{
	if (!c)
		return NULL;
	if (c->type==CELL)
		return CDR(c);
	return new_obj();
}
STACK(rplaca,2_ARGS)
obj_t *rplaca(obj_t *c,obj_t *v)
{
	if (!c||c->type!=CELL)
		return new_obj();
	decr_refs(CAR(c));
	CAR(c)=incr_refs(v);
	return c;
}
STACK(rplacd,2_ARGS)
obj_t *rplacd(obj_t *c,obj_t *v)
{
	if (!c||c->type!=CELL)
		return new_obj();
	decr_refs(CDR(c));
	CDR(c)=incr_refs(v);
	return c;
}
STACK(atom,1_ARG)
obj_t *atom(obj_t *obj)
{
	if (!obj)
		return new_symbol(strdup("T"));
	return obj->type==CELL?NULL:obj;
}
STACK(set,2_ARGS)
obj_t *set(obj_t *sym,obj_t *val)
{
	if (!sym||sym->type!=SYMBOL)
		return new_obj();
	set_binding(sym,val);
	return sym;
}
STACK(get,1_ARG)
obj_t *get(obj_t *sym)
{
	if (!sym||sym->type!=SYMBOL)
		return sym;
	return get_binding(sym);
}
STACK(unset,1_ARG)
obj_t *unset(obj_t *sym)
{
	if (!sym||sym->type==SYMBOL)
		unset_binding(sym);
	return NULL;
}
STACK(eq,2_ARGS)
obj_t *eq(obj_t *a,obj_t *b)
{
	if (eq_objs(a,b))
		return new_symbol(strdup("T"));
	else
		return NULL;
}
STACK(copy,1_ARG)
obj_t *copy(obj_t *obj)
{
	return copy_obj(obj);
}
STACK(length,1_ARG)
obj_t *length(obj_t *list)
{
	if (list->type!=CELL)
		return new_obj();
	return new_integer(list_length(list));
}
