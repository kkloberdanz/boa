#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "hindley-milner.h"
#include "typecheck.h"
#include "typeinfo.h"
#include "../util/util.h"
#include "../util/set.h"

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
    struct HMState state;
    hmstate_init(&state);
    while (ast) {
        typecheck_node(ast);
        ast = ast->sibling;
    }
    hmstate_free(&state);
    return 0;
}
