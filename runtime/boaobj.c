#include <stdio.h>
#include <stdlib.h>

#include "boaobj.h"

struct BoaObj *create_boa_int(long i) {
    struct BoaObj *obj = malloc(sizeof(struct BoaObj));
    obj->type = BOA_INT;
    obj->data.i = i;
    return obj;
}

struct BoaObj *perform_add(struct BoaObj *a, struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types");
        return NULL;
    }
    switch (a->type) {
        case BOA_NIL:
            dst = malloc(sizeof(struct BoaObj));
            break;

        case BOA_INT:
            dst = malloc(sizeof(struct BoaObj));
            dst->data.i = a->data.i + b->data.i;
            break;

        case BOA_FLOAT:
            dst = malloc(sizeof(struct BoaObj));
            dst->data.f = a->data.f + b->data.f;
            break;

        case BOA_STRING:
        case BOA_LIST:
        default:
            fprintf(stderr, "type not supported for add");
            return NULL;
    }
    dst->type = a->type;
    return dst;
}
