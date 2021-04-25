#ifndef BOAOBJ_H
#define BOAOBJ_H

#include <stdio.h>

enum BoaOp {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
};

enum BoaType {
    BOA_NIL,
    BOA_INT,
    BOA_FLOAT,
    BOA_BOOL,
    BOA_STRING,
    BOA_LIST,
    BOA_FUNC
};

union BoaObjData {
    long i;
    double f;
    char *s;
    struct BoaObj **l;
    char b;
    struct BoaObj *(*fn)(struct BoaObj *);
};

struct BoaObj {
    enum BoaType type;
    size_t len;
    size_t cap;
    union BoaObjData data;
};

void *boaobj_malloc(size_t size);
struct BoaObj *create_boa_int(long i);
struct BoaObj *create_boa_float(double f);
struct BoaObj *create_boa_string(const char *str);
struct BoaObj *create_boa_func(struct BoaObj *(*fn)(struct BoaObj *));
struct BoaObj *create_boa_nil(void);
struct BoaObj *create_boa_list(const size_t nmemb, ...);
void gc_init(void);
struct BoaObj *perform_add(const struct BoaObj *a, const struct BoaObj *b);
struct BoaObj *perform_mul(const struct BoaObj *a, const struct BoaObj *b);
struct BoaObj *perform_div(const struct BoaObj *a, const struct BoaObj *b);
struct BoaObj *perform_mod(const struct BoaObj *a, const struct BoaObj *b);
struct BoaObj *perform_sub(const struct BoaObj *a, const struct BoaObj *b);
struct BoaObj *perform_equ(const struct BoaObj *a, const struct BoaObj *b);
struct BoaObj *perform_lt(const struct BoaObj *a, const struct BoaObj *b);
struct BoaObj *perform_index(
    const struct BoaObj *list,
    const struct BoaObj *idx
);
struct BoaObj *perform_gt(const struct BoaObj *a, const struct BoaObj *b);

#endif /* BOAOBJ_H */
