#include <stdio.h>

#include "ast.h"
#include "typecheck.h"
#include "typeinfo.h"
#include "../util/util.h"

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
    while (ast) {
        typecheck_node(ast);
        ast = ast->sibling;
    }
    return 0;
}
