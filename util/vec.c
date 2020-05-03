#include <stdio.h>
#include <stdlib.h>

#include "vec.h"
#include "../compiler/typeinfo.h"

struct Vec *vec_new(void) {
    struct Vec *vec = malloc(sizeof(struct Vec));
    vec->last = 0;
    vec->capacity = 0;
    vec->data = NULL;
    return vec;
}

void vec_push(struct Vec *vec, TypeId id) {
    if (vec->last >= vec->capacity) {
        if (vec->capacity == 0) {
            vec->capacity = 2;
            vec->data = malloc(vec->capacity * sizeof(TypeId));
        } else {
            vec->capacity *= 2;
            vec->data = realloc(vec->data, vec->capacity * sizeof(TypeId));
        }
    }
    vec->data[vec->last] = id;
    vec->last++;
}

void vec_free(struct Vec *vec) {
    if (vec) {
        free(vec->data);
        free(vec);
    }
}

void vec_print(struct Vec *vec) {
    size_t i;
    putchar('[');
    for (i = 0; i < vec->last; i++) {
        if (i == vec->last - 1) {
            printf("%lu", vec->data[i]);
        } else {
            printf("%lu, ", vec->data[i]);
        }
    }
    putchar(']');
}

#ifdef IBA_VEC_TEST
int main(void) {
    size_t i;
    struct Vec *v = vec_new();
    for (i = 0; i < 100; i++) {
        vec_push(v, i);
    }

    vec_print(v);
    vec_free(v);
    return 0;
}
#endif
