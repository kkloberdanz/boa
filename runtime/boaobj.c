#include <stdio.h>
#include <stdlib.h>

#include "boaobj.h"
#include "../extern/tgc/tgc.h"

tgc_t gc;
static struct BoaObj **small_nums;

void *boaobj_malloc(size_t size) {
    /*return tgc_alloc(&gc, size);*/
    return malloc(size);
}

void gc_init() {
    long i;
    small_nums = boaobj_malloc(511 * sizeof(struct BoaObj *));
    for (i = -255; i <= 255; i++) {
        long index = i + 255;
        small_nums[index] = malloc(sizeof(struct BoaObj));
        small_nums[index]->type = BOA_INT;
        small_nums[index]->data.i = i;
    }
}

struct BoaObj *create_boa_int(long i) {
    struct BoaObj *obj = NULL;
    if (i >= -255 && i <= 255) {
        long index = i + 255;
        obj = small_nums[index];
    } else {
        obj = boaobj_malloc(sizeof(struct BoaObj));
        obj->type = BOA_INT;
        obj->data.i = i;
    }
    return obj;
}

struct BoaObj *create_boa_float(double f) {
    struct BoaObj *obj = boaobj_malloc(sizeof(struct BoaObj));
    obj->type = BOA_FLOAT;
    obj->data.f = f;
    return obj;
}

struct BoaObj *perform_add(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        return NULL;
    }
    switch (a->type) {
        case BOA_NIL:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            break;

        case BOA_INT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.i = a->data.i + b->data.i;
            break;

        case BOA_FLOAT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
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

struct BoaObj *perform_equ(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        return NULL;
    }
    switch (a->type) {
        case BOA_NIL:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            break;

        case BOA_INT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.b = a->data.i == b->data.i;
            break;

        case BOA_FLOAT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.b = a->data.f == b->data.f;
            break;

        case BOA_STRING:
        case BOA_LIST:
        default:
            fprintf(stderr, "type not supported for add");
            return NULL;
    }
    dst->type = BOA_BOOL;
    return dst;
}

struct BoaObj *perform_sub(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        return NULL;
    }
    switch (a->type) {
        case BOA_NIL:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            break;

        case BOA_INT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.i = a->data.i - b->data.i;
            break;

        case BOA_FLOAT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.f = a->data.f - b->data.f;
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

struct BoaObj *perform_div(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        return NULL;
    }
    switch (a->type) {
        case BOA_NIL:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            break;

        case BOA_INT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.i = a->data.i / b->data.i;
            break;

        case BOA_FLOAT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.f = a->data.f / b->data.f;
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

struct BoaObj *perform_mul(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        return NULL;
    }
    switch (a->type) {
        case BOA_NIL:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            break;

        case BOA_INT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.i = a->data.i * b->data.i;
            break;

        case BOA_FLOAT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.f = a->data.f * b->data.f;
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

struct BoaObj *perform_mod(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        return NULL;
    }
    switch (a->type) {
        case BOA_NIL:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            break;

        case BOA_INT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.i = a->data.i % b->data.i;
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

struct BoaObj *perform_lt(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        return NULL;
    }
    switch (a->type) {
        case BOA_NIL:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            break;

        case BOA_INT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.i = a->data.i < b->data.i;
            break;

        case BOA_FLOAT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.f = a->data.f < b->data.f;
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
