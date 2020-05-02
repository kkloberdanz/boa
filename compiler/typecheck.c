#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "typecheck.h"
#include "typeinfo.h"
#include "../util/util.h"
#include "../util/memory.h"
#include "../util/set.h"

/* data types ****************************************************************/
struct State {
    TypeId new_type;
    TypeId *concrete_types;
    struct Set **equiv_types;
    size_t capacity;
};

static void state_init(struct State *state) {
    size_t capacity = 1;
    size_t i;
    state->new_type = TYPE_NOT_CHECKED;
    state->capacity = capacity;
    state->concrete_types = malloc(sizeof(TypeId) * capacity);
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
}
/*****************************************************************************/

/* type equivalency **********************************************************/
static void add_equivalent_type(
    struct State *state,
    TypeId parent_type,
    TypeId equiv_type
) {
    TypeId greatest_type = MAX(parent_type, equiv_type);
    struct Set *same_type_as_parent;

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

        for (i = old_capacity; i < state->capacity; i++) {
            state->equiv_types[i] = NULL;
            state->concrete_types[i] = TYPE_NOT_CHECKED;
        }
    }

    same_type_as_parent = state->equiv_types[parent_type];

    state->equiv_types[parent_type] = set_insert(same_type_as_parent, equiv_type);
}
/*****************************************************************************/

/* internal state tracking ***************************************************/
static TypeId get_new_type(struct State *state) {
    state->new_type++;
    return state->new_type;
}
/*****************************************************************************/

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

    state_init(&state);
    while (ast) {
        typecheck_node(ast);
        ast = ast->sibling;
    }
    return 0;
}

#ifdef IBA_TYPECHECK_TEST
int main(void) {
    int i, j;
    struct State state;
    TypeId types[] = {1, 2, 3, 4};
    TypeId equivs[4][4] = {
        {4, 6, 0, 0},
        {7, 8, 9, 0},
        {1, 0, 0, 0},
        {0, 0, 0, 0}
    };

    state_init(&state);

    for (i = 0; i < 4; i++) {
        for (j = 0; equivs[i][j] != 0; j++) {
            TypeId equiv_type = equivs[i][j];
            printf("T(%lu) := T(%lu)\n", types[i], equiv_type);
            add_equivalent_type(&state, types[i], equiv_type);
        }
    }

    puts("");
    for (i = 0; i < 4; i++) {
        struct Set *set = state.equiv_types[types[i]];
        printf("T(%lu) := ", types[i]);
        set_print(set);
    }

    state_free(&state);
}
#endif
