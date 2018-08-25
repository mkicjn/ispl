#include "interpret.h"
obj_t *interpret_cond(obj_t *l)
{ // Returns list at COND_END
	l=CDR(l);
	bool ret=false;
	for (;!ret&&l&&!symbol_match(CAR(l),"COND_END");l=CDR(CDR(l))) {
		interpret(CAR(l));
		obj_t *r=pop();
		if (r) {
			interpret(CAR(CDR(l)));
			ret=true;
		}
		decr_refs(r);
	}
	if (!ret)
		push(NULL);
	while (l&&!symbol_match(CAR(l),"COND_END"))
		l=CDR(l);
	return l;
}
void interpret(obj_t *list)
{ // Requires RPN list as argument
	for (obj_t *o=list;o;o=CDR(o)) {
		obj_t *instr=CAR(o);
		if (!instr) {
			push(NULL);
		} else if (instr->type!=SYMBOL) {
			push(instr);
		} else if (instr==&list_sym||symbol_match(instr,"LIST")) {
			push(instr);
		} else if (instr==&list_end_sym||symbol_match(instr,"LIST_END")) {
			obj_t *body=NULL;
			while (!symbol_match(stack_obj(0),"LIST"))
				body=CONS(dpop(),body);
			drop();
			push(body);
		} else if (instr==&drop_sym||symbol_match(instr,"DROP")) {
			drop();
		} else if (instr==&exec_sym||symbol_match(instr,"!")) {
			obj_t *f=pop();
			if (!funcall(f))
				goto FATAL_INTERP_ERROR;
			decr_refs(f);
#ifdef ALL_ERRORS_FATAL
			obj_t *r=stack_obj(0);
			if (r&&r->type==ERROR) {
				drop();
				goto FATAL_INTERP_ERROR;
			}
#endif
		} else if (instr==&quote_sym||symbol_match(instr,"QUOTE")) {
			o=CDR(o);
			push(CAR(o));
		} else if (instr==&cond_sym||symbol_match(instr,"COND")) {
			o=interpret_cond(o);
			// COND_END skipped by o=CDR(o) in for-loop
		} else {
			push(get_binding(instr->data.sym));
		}
	}
	return;

FATAL_INTERP_ERROR:
	fputs("Fatal error while interpreting ",stderr);
	print_obj(list,stderr);
	fputc('\n',stderr);
	print_stack();
	exit(1);
}

