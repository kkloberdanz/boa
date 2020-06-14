#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typeinfo.h"
#include "../util/util.h"
#include "../util/set.h"
#include "ast.h"
#include "hindley-milner.h"

void hmstate_init(struct HMState *state) {
    size_t capacity = 1;
    size_t i;
    state->new_type = TYPE_LASTTYPE + 1;
    state->greatest_type = TYPE_LASTTYPE + 1;
    state->capacity = capacity;
    state->equiv_types = malloc(sizeof(struct Set) * capacity);

    for (i = 0; i < capacity; i++) {
        state->equiv_types[i] = NULL;
    }
}

void hmstate_free(struct HMState *state) {
    if (state) {
        size_t i;
        for (i = 0; i < state->capacity; i++) {
            struct Set *set = state->equiv_types[i];
            set_free(set);
        }

        free(state->equiv_types);
    }
}

void add_equivalent_type(
    struct HMState *state,
    TypeId parent_type,
    TypeId equiv_type
) {
    TypeId greatest_type = MAX(parent_type, equiv_type);

    if (greatest_type >= state->capacity) {
        size_t old_capacity = state->capacity;
        size_t i;

        state->capacity = greatest_type * 2;

        state->equiv_types = realloc(
            state->equiv_types,
            sizeof(struct Set) * state->capacity
        );

        for (i = old_capacity; i < state->capacity; i++) {
            state->equiv_types[i] = NULL;
        }
    }

    state->equiv_types[parent_type] = set_insert(
        state->equiv_types[parent_type],
        equiv_type
    );

    state->equiv_types[parent_type] = set_insert(
        state->equiv_types[parent_type],
        parent_type
    );

    state->greatest_type = MAX(state->greatest_type, greatest_type);
}

static void find_equiv_types(struct HMState *state, TypeId current_type) {
    struct Set *current_set = state->equiv_types[current_type];
    struct Vec *set_vec = set_to_vec(current_set);
    size_t i;
    size_t j;

    for (i = 0; i < set_vec->last; i++) {
        TypeId checking_type = set_vec->data[i];
        struct Set *equiv_set = state->equiv_types[checking_type];
        struct Vec *equiv_vec = set_to_vec(equiv_set);

        for (j = 0; j < equiv_vec->last; j++) {
            TypeId equiv_type = equiv_vec->data[j];
            set_insert(current_set, equiv_type);
        }

        state->equiv_types[checking_type] = set_insert(
            equiv_set,
            current_type
        );

        vec_free(equiv_vec);
    }
    vec_free(set_vec);
}

static void expand_types(struct HMState *state) {
    TypeId current_type;
    for (current_type = 0;
        current_type < state->greatest_type;
        current_type++
    ) {
        find_equiv_types(state, current_type);
    }
}

static int check_for_conflict(struct Vec *equiv_vec, TypeId concrete_type) {
    size_t i;
    for (i = 1; i < equiv_vec->last; i++) {
        TypeId equiv_type = equiv_vec->data[i];
        if (equiv_type < TYPE_LASTTYPE) {
            /* TODO:
             *     add line number and code snippet to error message
             */
            fprintf(
                stderr,
                "typecheck error, '%s' not compatible with '%s'\n",
                builtin_types[concrete_type],
                builtin_types[equiv_type]
            );
            return -1;
        }
    }
    return 0;
}

static int check_conflicting_type(struct HMState *state, TypeId current_type) {
    struct Set *current_set = state->equiv_types[current_type];
    struct Vec *set_vec = set_to_vec(current_set);
    size_t i;
    int error_code = 0;

    for (i = 0; i < set_vec->last; i++) {
        TypeId checking_type = set_vec->data[i];
        struct Set *equiv_set = state->equiv_types[checking_type];

        if (equiv_set) {
            struct Vec *equiv_vec = set_to_vec(equiv_set);
            TypeId concrete_type = equiv_vec->data[0];
            error_code = check_for_conflict(equiv_vec, concrete_type);
            vec_free(equiv_vec);
            if (error_code) {
                goto exit;
            }
        }
    }
exit:
    vec_free(set_vec);
    return error_code;
}

int check_conflicing_types(struct HMState *state) {
    TypeId current_type;
    int error_code = 0;
    for (current_type = 0;
        current_type < state->greatest_type;
        current_type++
    ) {
        error_code = check_conflicting_type(state, current_type);
        if (error_code) {
            return error_code;
        }
    }
    return error_code;
}

void collapse_types(struct HMState *state) {
    expand_types(state);
    expand_types(state);
}

TypeId get_new_type(struct HMState *state) {
    state->greatest_type = MAX(state->greatest_type, state->new_type);
    state->new_type++;
    return state->new_type;
}

void print_sets(
    struct HMState *state,
    const size_t num_base_types
) {
    size_t i;
    for (i = 0; i <= num_base_types; i++) {
        struct Set *set = state->equiv_types[i];
        printf("T(%lu) := ", i);
        set_print(set);
    }
}

/*
 * 1) Traverse AST, and give each expression a type, and populate these types
 *    in the TypeEquiv table.
 *    When a concrete type is found for a given temporary type (Tx),
 *    add it to the concrete types table.
 *
 * 2) Loop through TypeEquiv. For each type in the key's set, consolodate the
 *    sets from those types into the parent type, then delete their keys from
 *    the table TypeEquiv.
 *    When a concrete type is found, populate ConcreteTypes with this info
 *    tbl(key) = type
 *
 * 3) Iterate through AST again, and for each expression, replace its
 *    temporary type with a concrete type. If there does not exist an entry
 *    in the ConcreteTypes table for this type, then typechecking failed,
 *    and the user needs to fix their types. Report a diagnostic message
 *    informing which line and which identifier does not have a correct type.
 */

#ifdef BOA_HINDLEY_MILNER_TEST
int main(void) {
    int error_code = 0;
    const TypeId x = TYPE_LASTTYPE;
    size_t i, j;
    struct HMState state;
    TypeId types[] = {
        x + 1,
        x + 2,
        x + 3,
        x + 4,
        x + 5,
        x + 6,
        x + 7,
        x + 8,
        x + 9
    };
    const size_t num_base_types = sizeof(types) / sizeof(TypeId);
    TypeId equivs[9][4] = {
        {x + 4, x + 6, 0, 0},
        {x + 7, x + 8, x + 9, 0},
        {x + 1, 0, 0, 0},
        {0, 0, 0, 0},
        {x + 4, TYPE_INT, 0, 0},
        {x + 4, x + 5, 0, 0},
        {x + 2, TYPE_BOOL, 0, 0},
        {x + 2, 0, 0, 0},
#ifdef BOA_TEST_TYPE_ERROR
        {x + 2, TYPE_FLOAT, 0, 0}
#else
        {x + 2, 0, 0, 0}
#endif
    };

    hmstate_init(&state);

    printf("num_base_types: %lu\n", num_base_types);

    for (i = 0; i < num_base_types; i++) {
        for (j = 0; equivs[i][j] != 0; j++) {
            TypeId equiv_type = equivs[i][j];
            printf("T(%lu) := T(%lu)\n", types[i], equiv_type);
            add_equivalent_type(&state, types[i], equiv_type);
        }
    }

    puts("\nbefore collapse");
    print_sets(&state, num_base_types);

    /* collapse types */
    collapse_types(&state);
    puts("\nafter collapse");
    print_sets(&state, num_base_types);

    puts("typechecking...");

    error_code = check_conflicing_types(&state);
    if (error_code) {
        fprintf(stderr, "ERROR: invalid use of types\n");
    }

    hmstate_free(&state);
    return error_code;
}
#endif
