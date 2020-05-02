#include "../compiler/typeinfo.h"

struct Set {
    TypeId id;
    struct Set *left;
    struct Set *right;
};

void set_insert(struct Set *set, TypeId id);

void set_rec_print(struct Set *set);

void set_print(struct Set *set);

int set_contains(struct Set *set, TypeId id);

void set_init(struct Set *set);
