#ifndef __TYPE_
#define __TYPE_

#include <stdbool.h>
#include "kind.h"

enum ttype {
    /* type variable */
    TYPE_VARIABLE,
    /* type operator */
    TYPE_POLY, //over type with name 'name'
    TYPE_NAME,
    TYPE_FUNCTION, //arity = 2
    TYPE_APPL,
    TYPE_ABSTR
};

struct type {
    enum ttype t;
    int arity;
    char *name;
    struct kind *kind;
    union {
        struct type *args[2];
        int index;
    };
};

struct type *mktype(enum ttype,char *,int);
struct type *type_function(struct type *,struct type *);
struct type *type_appl(struct type *,struct type *);
struct type *type_var(char *);
struct type *type_poly(char *,struct type *,struct kind *);
struct type *type_abstr(char *,struct type *,struct kind *);
void printtype(struct type *);
struct type *duptype(struct type *);
bool cmptype(struct type *,struct type *);
void destroytype(struct type *);

#endif
