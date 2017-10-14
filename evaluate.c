#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lambdaexpr.h"

static struct lambdaexpr *copytree(const struct lambdaexpr *const orig,const int subst) {
	if (!orig) return NULL;
	struct lambdaexpr *copy=mknode(orig->type);
	if (!copy) return NULL;
	switch(orig->type) {
		case TERM:
			copy->term.s=(char *)malloc((strlen(orig->term.s)+1)*sizeof(char));
			if (!copy->term.s) goto FAIL;
			strcpy(copy->term.s,orig->term.s);
			copy->term.index=orig->term.index+(orig->term.index?subst:0);
			break;
		case ABSTR:
			copy->abstr.id=(char *)malloc((strlen(orig->abstr.id)+1)*sizeof(char));
			if (!copy->abstr.id) goto FAIL;
			strcpy(copy->abstr.id,orig->abstr.id);
			copy->abstr.expr=copytree(orig->abstr.expr,subst);
			break;
		case APPL:
			copy->appl.lhs=copytree(orig->appl.lhs,subst);
			copy->appl.rhs=copytree(orig->appl.rhs,subst);
			break;
		default:
		FAIL:
			free(copy);
			copy=NULL;
			break;
	}
	return copy;
}

static struct lambdaexpr *substitute(struct lambdaexpr *source,struct lambdaexpr *by,int index) {
	if (!source || !by/* || source->type!=ABSTR*/) return NULL;
	struct lambdaexpr *result=NULL;//=source->abstr.expr;
	switch(source->type) {
		case TERM:;
			if (source->term.index==index) result=copytree(by,index-1); //increase values of bounded variables by index
			else result=copytree(source,0);
			break;
		case APPL:;
			struct lambdaexpr *nlhs=substitute(source->appl.lhs,by,index);
			struct lambdaexpr *nrhs=substitute(source->appl.rhs,by,index);
			result=mknode(APPL);
			result->appl.lhs=nlhs;
			result->appl.rhs=nrhs;
			break;
		case ABSTR:;
			struct lambdaexpr *nexpr=substitute(source->abstr.expr,by,index+1);
			result=mknode(ABSTR);
			result->abstr.id=strdup(source->abstr.id);
			result->abstr.expr=nexpr;
			break;
		default: return NULL;
	}
	return result;
}

static struct lambdaexpr *seval(struct lambdaexpr *l) {
	struct lambdaexpr *m=NULL,*n=NULL;
	if (l && l->type==APPL) {
		if ((l->appl.lhs && l->appl.lhs->type==ABSTR) && (l->appl.rhs && (l->appl.rhs->type==ABSTR || l->appl.rhs->type==TERM)))
			m=substitute(l->appl.lhs->abstr.expr,l->appl.rhs,1);
		else if (l->appl.lhs->type==APPL) {
			n=seval(l->appl.lhs);
			m=mknode(APPL);
			m->appl.lhs=n;
			m->appl.rhs=copytree(l->appl.rhs,0);
			n=NULL;
		} else {
			n=seval(l->appl.rhs);
			m=mknode(APPL);
			m->appl.rhs=n;
			m->appl.lhs=copytree(l->appl.lhs,0);
			n=NULL;
		}
	} else return copytree(l,0);
	return m;
}

struct lambdaexpr *eval(struct lambdaexpr *l) {
	struct lambdaexpr *m=l,*n=NULL;
	do {
		n=m;
		m=seval(m);
		destroynode(n);
	} while (m->type!=TERM && m->type!=ABSTR);
	return m;
}

void printnode(struct lambdaexpr *ast) {
	if(!ast) return;
	switch(ast->type) {
		case APPL:
			printf("(");
			printnode(ast->appl.lhs);
			printf(" ");
			printnode(ast->appl.rhs);
			printf(")");
			break;
		case ABSTR:
			printf("\\%s.",ast->abstr.id);
			printnode(ast->abstr.expr);
			break;
		case TERM:
			printf("%s(%d) ",ast->term.s,ast->term.index);
			break;
		default: return;
	}
}


