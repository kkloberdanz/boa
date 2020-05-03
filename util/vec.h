#include <stdio.h>
#include <stdlib.h>

#include "../compiler/typeinfo.h"

struct Vec {
    TypeId *data;
    size_t last;
    size_t capacity;
};

struct Vec *vec_new();

void vec_push(struct Vec *vec, TypeId id);

void vec_free(struct Vec *vec);
