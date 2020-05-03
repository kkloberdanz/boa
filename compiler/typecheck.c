#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "typecheck.h"
#include "typeinfo.h"
#include "../util/util.h"
#include "../util/set.h"

struct State {
    size_t capacity;
    TypeId new_type;
    TypeId greatest_type;
    TypeId *concrete_types;
    TypeId *reduced_to_type;
    struct Set **equiv_types;
};

static void state_init(struct State *state) {
    size_t capacity = 1;
    size_t i;
    state->new_type = TYPE_NOT_CHECKED;
    state->greatest_type = TYPE_NOT_CHECKED;
    state->capacity = capacity;
    state->concrete_types = malloc(sizeof(TypeId) * capacity);
    state->reduced_to_type = malloc(sizeof(TypeId) * capacity);
    state->equiv_types = malloc(sizeof(struct Set) * capacity);

    for (i = 0; i < capacity; i++) {
        state->equiv_types[i] = NULL;
        state->concrete_types[i] = TYPE_NOT_CHECKED;
    }
}

static void state_free(struct State *state) {
    size_t i;
    for (i = 0; i < state->capacity; i++) {
        struct Set *set = state->equiv_types[i];
        set_free(set);
    }

    free(state->equiv_types);
    free(state->concrete_types);
    free(state->reduced_to_type);
}

static void add_equivalent_type(
    struct State *state,
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

        state->concrete_types = realloc(
            state->concrete_types,
            sizeof(TypeId) * state->capacity
        );

        state->reduced_to_type = realloc(
            state->reduced_to_type,
            sizeof(TypeId) * state->capacity
        );

        for (i = old_capacity; i < state->capacity; i++) {
            state->equiv_types[i] = NULL;
            state->concrete_types[i] = TYPE_NOT_CHECKED;
            state->reduced_to_type[i] = TYPE_NOT_CHECKED;
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

static void find_equiv_types(struct State *state, TypeId current_type) {
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

static void expand_types(struct State *state) {
    TypeId current_type;
    for (current_type = 0;
        current_type < state->greatest_type;
        current_type++
    ) {
        find_equiv_types(state, current_type);
    }
}

static void collapse_types(struct State *state) {
    expand_types(state);
    expand_types(state);
}

static TypeId get_new_type(struct State *state) {
    state->greatest_type = MAX(state->greatest_type, state->new_type);
    state->new_type++;
    return state->new_type;
}

static void typecheck_node(ASTNode *ast) {
    switch (ast->kind) {
        case FUNC_CALL:
            break;

        case ASSIGN_EXPR:
            break;

        case OPERATOR:
            break;

        case RETURN_STMT:
            break;

        case LOAD_STMT:
            break;

        case CONDITIONAL:
            break;

        case FUNC_DEF:
            break;
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
int typecheck(ASTNode *ast) {
    struct State state;

    UNUSED(add_equivalent_type);
    UNUSED(get_new_type);
    UNUSED(collapse_types);

    state_init(&state);
    while (ast) {
        typecheck_node(ast);
        ast = ast->sibling;
    }

    state_free(&state);
    return 0;
}

static void print_sets(
    struct State *state,
    const size_t num_base_types,
    TypeId types[]
) {
    size_t i;
    for (i = 0; i < num_base_types; i++) {
        struct Set *set = state->equiv_types[types[i]];
        printf("T(%lu) := ", types[i]);
        set_print(set);
    }
}

#ifdef IBA_TYPECHECK_TEST
int main(void) {
    size_t i, j;
    struct State state;
    TypeId types[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    const size_t num_base_types = sizeof(types) / sizeof(TypeId);
    TypeId equivs[9][4] = {
        {4, 6, 0, 0},
        {7, 8, 9, 0},
        {1, 0, 0, 0},
        {0, 0, 0, 0},
        {4, 3, 0, 0},
        {4, 5, 0, 0},
        {2, 0, 0, 0},
        {2, 0, 0, 0},
        {2, 0, 0, 0}
    };

    state_init(&state);

    printf("num_base_types: %lu\n", num_base_types);

    for (i = 0; i < num_base_types; i++) {
        for (j = 0; equivs[i][j] != 0; j++) {
            TypeId equiv_type = equivs[i][j];
            printf("T(%lu) := T(%lu)\n", types[i], equiv_type);
            add_equivalent_type(&state, types[i], equiv_type);
        }
    }

    puts("\nbefore collapse");
    print_sets(&state, num_base_types, types);

    /* collapse types */
    collapse_types(&state);
    puts("\nafter collapse");
    print_sets(&state, num_base_types, types);

    state_free(&state);
}
#endif
