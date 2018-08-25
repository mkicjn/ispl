#include "core.h"
obj_t *print(obj_t *obj)
{
	print_obj(obj,stdout);
	return obj;
}
STACK(print,1)
obj_t *terpri(void)
{
	putchar('\n');
	return NULL;
}
STACK(terpri,0)
obj_t *cons(obj_t *a,obj_t *b)
{
	return CONS(a,b);
}
STACK(cons,2)
obj_t *car(obj_t *c)
{
	if (!c)
		return NULL;
	if (!type_check(c,CELL,"CAR: "))
		return new_object();
	return CAR(c);
}
STACK(car,1)
obj_t *cdr(obj_t *c)
{
	if (!c)
		return NULL;
	if (!type_check(c,CELL,"CDR: "))
		return new_object();
	return CDR(c);
}
STACK(cdr,1)
obj_t *rplaca(obj_t *c,obj_t *v)
{
	if (!type_check(c,CELL,"RPLACA: "))
		return new_object();
	decr_refs(CAR(c));
	CAR(c)=incr_refs(v);
	return c;
}
STACK(rplaca,2)
obj_t *rplacd(obj_t *c,obj_t *v)
{
	if (!type_check(c,CELL,"RPLACD: "))
		return new_object();
	decr_refs(CDR(c));
	CDR(c)=incr_refs(v);
	return c;
}
STACK(rplacd,2)
obj_t *atom(obj_t *obj)
{
	if (!obj||obj->type!=CELL)
		return incr_refs(&t_sym);
	return NULL;
}
STACK(atom,1)
obj_t *set(obj_t *sym,obj_t *val)
{
	if (!type_check(sym,SYMBOL,"SET: "))
		return new_object();
	set_binding(sym->data.sym,val);
	return val;
}
STACK(set,2)
obj_t *get(obj_t *sym)
{
	if (!sym||sym->type!=SYMBOL)
		return sym;
	return get_binding(sym->data.sym);
}
STACK(get,1)
obj_t *unset(obj_t *sym)
{
	if (!sym||sym->type==SYMBOL)
		unset_binding(sym->data.sym);
	return NULL;
}
STACK(unset,1)
obj_t *eq(obj_t *a,obj_t *b)
{
	if (eq_objs(a,b))
		return incr_refs(&t_sym);
	else
		return NULL;
}
STACK(eq,2)
obj_t *copy(obj_t *obj)
{
	return copy_obj(obj);
}
STACK(copy,1)
obj_t *length(obj_t *list)
{
	if (!type_check(list,NIL|CELL,"LENGTH: "))
		return new_object();
	return new_integer(list_length(list));
}
STACK(length,1)
obj_t *lambda(obj_t *args,obj_t *body)
{
	bool type_err=false;
	type_err|=!type_check(args,NIL|CELL,"LAMBDA, args: ");
	for (obj_t *o=args;o;o=CDR(o))
		type_err|=!type_check(CAR(o),SYMBOL,"LAMBDA, arg: ");
	type_err|=!type_check(body,CELL|SYMBOL,"LAMBDA, body: ");
	if (type_err)
		return new_object();
	return new_lispfunction(args,rpn(body));
}
STACK(lambda,2)
obj_t *eval(obj_t *expr)
{
	obj_t *r=rpn(expr);
	incr_refs(r);
	interpret(r);
	decr_refs(r);
	return dpop();
}
STACK(eval,1)
obj_t *lread(obj_t *n)
{
	if (!type_check(n,INTEGER,"READ: "))
		return new_object();
	int l=n->data.i;
	char buf[l];
	fgets(buf,l,stdin);
	return read_str(buf);
}
STACK(lread,1)
obj_t *null(obj_t *c)
{
	return !c?incr_refs(&t_sym):NULL;
}
STACK(null,1)
obj_t *quit(void)
{
	exit(0);
}
STACK(quit,0)
obj_t *see(obj_t *func)
{
	if (!type_check(func,FUNCTION,"SEE: "))
		return new_object();
	if (!func->data.func.lambda)
		return NULL;
	return func->data.func.rep.lisp;
}
STACK(see,1)
static struct timespec last_tick;
obj_t *tick(void)
{
	clock_gettime(CLOCK_MONOTONIC,&last_tick);
	return NULL;
}
STACK(tick,0)
obj_t *tock(void)
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC,&now);
	double dt=now.tv_sec-last_tick.tv_sec;
	dt+=(now.tv_nsec-last_tick.tv_nsec)/1e9;
	return new_double(dt);
}
STACK(tock,0)
obj_t *uplevel(obj_t *n,obj_t *expr)
{
	bool type_err=false;
	type_err|=!type_check(n,INTEGER,"UPLEVEL, arg 1: ");
	type_err|=!type_check(expr,CELL|SYMBOL,"UPLEVEL, arg 2: ");
	if (type_err)
		return new_object();
	if (level<1) {
		puts("UPLEVEL: No higher level");
		return new_object();
	}
	table_t *loc=pop_namespace();
	obj_t *t=incr_refs(rpn(expr));
	interpret(t);
	decr_refs(t);
	push_namespace(loc);
	return dpop();
}
STACK(uplevel,2)
obj_t *nconc(obj_t *a,obj_t *b)
{
	if (!type_check(a,CELL,"NCONC: "))
		return new_object();
	concatenate(a,b);
	return a;
}
STACK(nconc,2)
obj_t *lfor(obj_t *init,obj_t *cond,obj_t *iter,obj_t *body)
{
	bool type_err=false;
	type_err|=!type_check(init,NIL|CELL,"FOR, initialization: ");
	type_err|=!type_check(init,NIL|CELL,"FOR, condition: ");
	type_err|=!type_check(iter,NIL|CELL,"FOR, iteration: ");
	type_err|=!type_check(body,CELL,"FOR, body: ");
	if (type_err)
		return new_object();
	init=incr_refs(rpn(init));
	cond=incr_refs(rpn(cond));
	iter=incr_refs(rpn(iter));
	body=incr_refs(rpn(body));
	interpret(init);
	decr_refs(init);
	drop();
	obj_t *ret=NULL;
	obj_t *c;
	for (;;) {
		if (cond) {
			interpret(cond);
			c=pop();
			if (c)
				decr_refs(c);
			else
				break;
		}
		interpret(body);
		decr_refs(ret);
		ret=pop();
		interpret(iter);
		drop();
	}
	decr_refs(cond);
	decr_refs(body);
	decr_refs(iter);
	if (ret)
		ret->refs--;
	return ret;
}
STACK(lfor,4)
obj_t *foreach(obj_t *name,obj_t *list,obj_t *body)
{
	bool type_err=false;
	type_err|=!type_check(name,SYMBOL,"FOREACH, name: ");
	type_err|=!type_check(list,CELL,"FOREACH, list: ");
	type_err|=!type_check(body,CELL,"FOREACH, body: ");
	if (type_err)
		return new_object();
	body=incr_refs(rpn(body));
	obj_t *ret=NULL;
	for (obj_t *o=list;o;o=CDR(o)) {
		set_binding(name->data.sym,CAR(o));
		interpret(body);
		decr_refs(ret);
		ret=pop();
	}
	decr_refs(body);
	if (ret)
		ret->refs--;
	return ret;
}
STACK(foreach,3)
obj_t *load(obj_t *file)
{
	if (!type_check(file,SYMBOL,"LOAD: "))
		return new_object();
	obj_t *r=load_file(file->data.sym);
	return r?r:new_object();
}
STACK(load,1)
obj_t *namespace(void)
{
	return new_namespace_obj(new_namespace_table(NAMESPACE_SIZE));
}
STACK(namespace,0)
obj_t *inside(obj_t *namespace,obj_t *expr)
{
	bool type_err=false;
	type_err|=!type_check(namespace,NAMESPACE,"INSIDE, namespace: ");
	type_err|=!type_check(expr,CELL|SYMBOL,"INSIDE, expression: ");
	if (type_err)
		return new_object();
	obj_t *t=incr_refs(rpn(expr));
	push_namespace(namespace->data.table);
	interpret(t);
	decr_refs(t);
	pop_namespace();
	return dpop();
}
STACK(inside,2)
obj_t *l_typeof(obj_t *obj)
{
	if (!obj)
		return NULL;
	switch (obj->type) {
	case CELL:
		return &cell_sym;
	case DOUBLE:
		return &double_sym;
	case ERROR:
		return &error_sym;
	case FUNCTION:
		return &function_sym;
	case NAMESPACE:
		return &namespace_sym;
	case INTEGER:
		return &integer_sym;
	case SYMBOL:
		return &symbol_sym;
	default:
		return new_object();
	}
}
STACK(l_typeof,1)
obj_t *append(obj_t *a,obj_t *b)
{
	if (!type_check(a,CELL,"APPEND: "))
		return new_object();
	a=copy_obj(a);
	concatenate(a,b);
	return a;
}
STACK(append,2)
obj_t *l_interpret(obj_t *sbe)
{ // Takes a stack-based expression
	interpret(sbe);
	return NULL;
}
STACK(l_interpret,1)
obj_t *stack(void)
{ // Returns number of stack items
	return new_integer(print_stack());
}
STACK(stack,0)
void init_core(void)
{
	insert(dict,"T",incr_refs(&t_sym));
	INIT(APPEND,append)
	INIT(ATOM,atom)
	INIT(CAR,car)
	INIT(CDR,cdr)
	INIT(CONS,cons)
	INIT(COPY,copy)
	INIT(EQ,eq)
	INIT(EVAL,eval)
	INIT(EXIT,quit)
	INIT(FOR,lfor)
	INIT(FOREACH,foreach)
	INIT(GET,get)
	INIT(INSIDE,inside)
	INIT(INTERPRET,l_interpret)
	INIT(LAMBDA,lambda)
	INIT(LENGTH,length)
	INIT(LOAD,load)
	INIT(NAMESPACE,namespace)
	INIT(NCONC,nconc)
	INIT(NOT,null)
	INIT(NULL,null)
	INIT(PRINT,print)
	INIT(QUIT,quit)
	INIT(READ,lread)
	INIT(RPLACA,rplaca)
	INIT(RPLACD,rplacd)
	INIT(SEE,see)
	INIT(SET,set)
	INIT(STACK,stack)
	INIT(TERPRI,terpri)
	INIT(TICK,tick)
	INIT(TOCK,tock)
	INIT(TYPEOF,l_typeof)
	INIT(UNSET,unset)
	INIT(UPLEVEL,uplevel)
}
