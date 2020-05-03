#ifndef SET_H
#define SET_H

#include "../compiler/typeinfo.h"

struct Set {
    TypeId id;
    struct Set *left;
    struct Set *right;
};

struct Set *set_insert(struct Set *root, TypeId id);

void set_print(struct Set *set);

int set_contains(struct Set *set, TypeId id);

struct Set *set_new(TypeId id);

void set_free(struct Set *set);

TypeId *set_smallest(struct Set *set);

#endif /* SET_H */
