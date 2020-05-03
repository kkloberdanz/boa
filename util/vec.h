#ifndef VEC_H
#define VEC_H

#include <stdio.h>
#include <stdlib.h>

#include "../compiler/typeinfo.h"

struct Vec {
    TypeId *data;
    size_t last;
    size_t capacity;
};

struct Vec *vec_new(void);

void vec_push(struct Vec *vec, TypeId id);

void vec_free(struct Vec *vec);

void vec_print(struct Vec *vec);

#endif
