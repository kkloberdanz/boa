#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "boaobj.h"

static struct BoaObj **small_nums;
static struct BoaObj *nil_obj;

static const int SMALL_NUMS_START = -255;
static const int SMALL_NUMS_END = 255;

char *strdup(const char *);

void *boaobj_malloc(size_t size) {
    /* TODO: add to garbage collector's collection */
    return malloc(size);
}

void gc_init() {
    long i;
    size_t nmemb = 1 + (size_t)abs(SMALL_NUMS_START) + (size_t)abs(SMALL_NUMS_END);
    size_t sz = nmemb * sizeof(struct BoaObj *);
    nil_obj = boaobj_malloc(sizeof(struct BoaObj));
    nil_obj->type = BOA_NIL;
    small_nums = boaobj_malloc(sz);
    for (i = SMALL_NUMS_START; i <= SMALL_NUMS_END; i++) {
        long index = i + SMALL_NUMS_END;
        small_nums[index] = malloc(sizeof(struct BoaObj));
        small_nums[index]->type = BOA_INT;
        small_nums[index]->data.i = i;
    }
}

struct BoaObj *create_boa_nil(void) {
    return nil_obj;
}

struct BoaObj *create_boa_int(long i) {
    struct BoaObj *obj = NULL;
    if (i >= SMALL_NUMS_START && i <= SMALL_NUMS_END) {
        long index = i + SMALL_NUMS_END;
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

struct BoaObj *create_boa_string(const char *str) {
    struct BoaObj *obj = boaobj_malloc(sizeof(struct BoaObj));
    obj->type = BOA_STRING;
    obj->data.s = strdup(str);
    return obj;
}

struct BoaObj *create_boa_func(struct BoaObj *(*fn)(struct BoaObj *)) {
    struct BoaObj *obj = boaobj_malloc(sizeof(struct BoaObj));
    obj->type = BOA_FUNC;
    obj->data.fn = fn;
    return obj;
}

struct BoaObj *create_boa_list(const size_t nmemb, ...) {
    va_list valist;
    struct BoaObj *obj = boaobj_malloc(sizeof(struct BoaObj));
    size_t i;

    va_start(valist, nmemb);
    obj->type = BOA_LIST;
    obj->len = nmemb;
    obj->cap = nmemb;
    obj->data.l = boaobj_malloc(sizeof(struct BoaObj *) * nmemb);
    for (i = 0; i < nmemb; i++) {
        obj->data.l[i] = va_arg(valist, struct BoaObj *);
    }
    return obj;
}

struct BoaObj *perform_add(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        exit(EXIT_FAILURE);
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

        case BOA_STRING: {
            size_t len = strlen(a->data.s) + strlen(b->data.s) + 1;
            char *buf = malloc(len);
            dst = boaobj_malloc(sizeof(struct BoaObj));
            strcpy(buf, a->data.s);
            strcat(buf, b->data.s);
            dst->data.s = buf;
            break;
        }

        case BOA_LIST: {
            size_t len = a->len + b->len;
            dst = malloc(sizeof(struct BoaObj));
            dst->data.l = malloc(sizeof(struct BoaObj **) * len);
            dst->len = len;
            dst->cap = len;
            memcpy(dst->data.l, a->data.l, a->len * sizeof(struct BoaObj *));
            memcpy(
                dst->data.l + a->len,
                b->data.l,
                b->len * sizeof(struct BoaObj *)
            );
            break;
        }

        case BOA_FUNC:
        case BOA_BOOL:
            fprintf(stderr, "type not supported\n");
            exit(EXIT_FAILURE);
    }
    dst->type = a->type;
    return dst;
}

struct BoaObj *perform_equ(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        exit(EXIT_FAILURE);
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

        case BOA_BOOL:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.b = a->data.b == b->data.b;
            break;

        case BOA_STRING:
        case BOA_LIST:
        case BOA_FUNC:
            fprintf(stderr, "type not supported\n");
            exit(EXIT_FAILURE);
    }
    dst->type = BOA_BOOL;
    return dst;
}

struct BoaObj *perform_sub(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        exit(EXIT_FAILURE);
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

        case BOA_BOOL:
        case BOA_STRING:
        case BOA_LIST:
        case BOA_FUNC:
            fprintf(stderr, "type not supported\n");
            exit(EXIT_FAILURE);
    }
    dst->type = a->type;
    return dst;
}

struct BoaObj *perform_div(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        exit(EXIT_FAILURE);
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

        case BOA_BOOL:
        case BOA_STRING:
        case BOA_LIST:
        case BOA_FUNC:
            fprintf(stderr, "type not supported\n");
            exit(EXIT_FAILURE);
    }
    dst->type = a->type;
    return dst;
}

struct BoaObj *perform_mul(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        exit(EXIT_FAILURE);
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

        case BOA_BOOL:
        case BOA_STRING:
        case BOA_LIST:
        case BOA_FUNC:
            fprintf(stderr, "type not supported\n");
            exit(EXIT_FAILURE);
    }
    dst->type = a->type;
    return dst;
}

struct BoaObj *perform_mod(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        exit(EXIT_FAILURE);
    }
    switch (a->type) {
        case BOA_NIL:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            break;

        case BOA_INT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.i = a->data.i % b->data.i;
            break;

        case BOA_BOOL:
        case BOA_STRING:
        case BOA_LIST:
        case BOA_FUNC:
        case BOA_FLOAT:
            fprintf(stderr, "type not supported\n");
            exit(EXIT_FAILURE);
    }
    dst->type = a->type;
    return dst;
}

struct BoaObj *perform_lt(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        exit(EXIT_FAILURE);
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

        case BOA_BOOL:
        case BOA_STRING:
        case BOA_LIST:
        case BOA_FUNC:
            fprintf(stderr, "type not supported\n");
            exit(EXIT_FAILURE);
    }
    dst->type = a->type;
    return dst;
}

struct BoaObj *perform_index(
    const struct BoaObj *list,
    const struct BoaObj *idx
) {
    struct BoaObj *dst = NULL;
    size_t index = idx->data.i;


    if (list->len < index) {
        fprintf(stderr, "list index out of range\n");
        exit(EXIT_FAILURE);
    }
    if (list->type != BOA_LIST) {
        fprintf(stderr, "can only take an index of a list\n");
        exit(EXIT_FAILURE);
    }
    if (idx->type != BOA_INT) {
        fprintf(stderr, "list index must be an int\n");
        exit(EXIT_FAILURE);
    }
    dst = list->data.l[index];
    return dst;
}

struct BoaObj *perform_gt(const struct BoaObj *a, const struct BoaObj *b) {
    struct BoaObj *dst = NULL;
    if (a->type != b->type) {
        fprintf(stderr, "mismatched types\n");
        exit(EXIT_FAILURE);
    }
    switch (a->type) {
        case BOA_NIL:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            break;

        case BOA_INT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.i = a->data.i > b->data.i;
            break;

        case BOA_FLOAT:
            dst = boaobj_malloc(sizeof(struct BoaObj));
            dst->data.f = a->data.f > b->data.f;
            break;

        case BOA_STRING:
        case BOA_LIST:
        default:
            fprintf(stderr, "type not supported\n");
            exit(EXIT_FAILURE);
    }
    dst->type = a->type;
    return dst;
}
