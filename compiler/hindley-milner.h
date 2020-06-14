#ifndef HINDLEY_MILNER_H
#define HINDLEY_MILNER_H

#include "ast.h"

struct HMState {
    size_t capacity;
    TypeId new_type;
    TypeId greatest_type;
    struct Set **equiv_types;
    TypeId *final_types;
};

void hmstate_init(struct HMState *state);

void hmstate_free(struct HMState *state);

TypeId get_new_type(struct HMState *state);

void add_equivalent_type(
    struct HMState *state,
    TypeId parent_type,
    TypeId equiv_type
);

void print_sets(
    struct HMState *state,
    const size_t num_base_types
);

void collapse_types(struct HMState *state);

int check_conflicing_types(struct HMState *state);

#endif /* HINDLEY_MILNER_H */
