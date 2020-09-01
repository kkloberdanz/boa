#ifndef BOAOBJ_H
#define BOAOBJ_H

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
    BOA_LIST
};

struct BoaObj {
    enum BoaType type;
    union {
        long i;
        double f;
        char *s;
        struct BoaObj *l;
        char b;
    } data;
};

struct BoaObj *create_boa_int(long i);
struct BoaObj *create_boa_float(double f);
struct BoaObj *create_boa_string(const char *str);
struct BoaObj *create_boa_nil();
void gc_init();
struct BoaObj *perform_add(const struct BoaObj *a, const struct BoaObj *b);
struct BoaObj *perform_mul(const struct BoaObj *a, const struct BoaObj *b);
struct BoaObj *perform_div(const struct BoaObj *a, const struct BoaObj *b);
struct BoaObj *perform_mod(const struct BoaObj *a, const struct BoaObj *b);
struct BoaObj *perform_sub(const struct BoaObj *a, const struct BoaObj *b);
struct BoaObj *perform_equ(const struct BoaObj *a, const struct BoaObj *b);
struct BoaObj *perform_lt(const struct BoaObj *a, const struct BoaObj *b);

#endif /* BOAOBJ_H */
