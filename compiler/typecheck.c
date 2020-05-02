#include <stdio.h>

#include "ast.h"
#include "typecheck.h"
#include "typeinfo.h"
#include "../util/util.h"
#include "../util/memory.h"
#include "../util/set.h"
#include "../extern/uthash/include/uthash.h"

/* data types ****************************************************************/
struct EquivTypesSet {
    TypeId type_id; /* hash key */
    UT_hash_handle hh; /* prereq for uthash */
};

struct State {
    TypeId new_type;
    TypeId *concrete_types;
    struct Set *equiv_types;
    size_t num_types;
    size_t capacity;
};
/*****************************************************************************/

struct IdentifierTypes {
    char *identifier; /* hash key */
    TypeId type_id;
    UT_hash_handle hh; /* prereq for uthash */
};

/* type equivalency **********************************************************/
static void add_equivalent_type(
    TypeId parent_type,
    TypeId equiv_type,
    struct State *state
) {
    struct Set *same_type_as_parent = &state->equiv_types[parent_type];

    if (state->num_types < state->capacity) {
        size_t new_size;
        state->capacity *= 2;
        new_size = sizeof(struct Set) * state->capacity;
        state->equiv_types = iba_realloc(state->equiv_types, new_size);
    }

    set_insert(same_type_as_parent, equiv_type);
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
    UNUSED(state);
    UNUSED(get_new_type);

    state.new_type = TYPE_LASTTYPE;
    state.num_types = 0;
    while (ast) {
        typecheck_node(ast);
        ast = ast->sibling;
    }
    return 0;
}
