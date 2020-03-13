#include <stdio.h>
#include <uthash.h>

#include "ast.h"
#include "typecheck.h"
#include "typeinfo.h"
#include "../util/util.h"

struct EquivTypesSet {
    TypeId type_id; /* hash key */
    UT_hash_handle hh; /* prereq for uthash */
};

struct TypeEquiv {
    TypeId type_id; /* hash key */
    struct EquivTypesSet equiv_types;
    UT_hash_handle hh; /* prereq for uthash */
};

struct SymbolToType {
    char *symbol; /* hash key */
    TypeId type_id;
    UT_hash_handle hh; /* prereq for uthash */
};

static void add_equivalent_type(struct TypeEquiv *tbl, TypeId type_id) {
    UNUSED(tbl);
    UNUSED(type_id);
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

int typecheck(ASTNode *ast) {
    UNUSED(add_equivalent_type);
    while (ast) {
        typecheck_node(ast);
        ast = ast->sibling;
    }
    return 0;
}
