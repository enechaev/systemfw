#include <stdlib.h>
#include <stdio.h>

#include "lambdaexpr.h"

struct lambdaexpr *mknode(enum nodetype t) {
	struct lambdaexpr *le=(struct lambdaexpr *)malloc(sizeof(struct lambdaexpr));
	if (!le) return NULL;
	switch(t) {
		case APPL:
			le->appl.lhs=NULL;
			le->appl.rhs=NULL; break;
		case ABSTR:
			le->abstr.id=NULL;
			le->abstr.expr=NULL; break;
		case TERM:
			le->term=NULL; break;
		default: return NULL;
	}
	le->type=t;
	return le;
}

int destroynode(struct lambdaexpr *le) {
	printf("x:%d\n",le->type);
	if (!le) return 1;
	switch (le->type) {
		case APPL:
			destroynode(le->appl.lhs);
			destroynode(le->appl.rhs);
			break;
		case ABSTR:
			destroynode(le->abstr.id);
			destroynode(le->abstr.expr);
			break;
		case TERM:
			if (le->term) free(le->term);
			break;
		default: return 1;
	}
	free(le);
	return 0;
}