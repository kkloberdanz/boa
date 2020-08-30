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
    } data;
};

struct BoaObj *create_boa_int(long i);
struct BoaObj *perform_add(struct BoaObj *a, struct BoaObj *b);
struct BoaObj *perform_mul(struct BoaObj *a, struct BoaObj *b);
struct BoaObj *perform_div(struct BoaObj *a, struct BoaObj *b);
struct BoaObj *perform_mod(struct BoaObj *a, struct BoaObj *b);
struct BoaObj *perform_sub(struct BoaObj *a, struct BoaObj *b);
struct BoaObj *perform_equ(struct BoaObj *a, struct BoaObj *b);

#endif /* BOAOBJ_H */
