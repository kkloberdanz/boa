#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "hindley-milner.h"
#include "typecheck.h"
#include "typeinfo.h"
#include "../util/util.h"
#include "../util/set.h"

static int mark_types(struct HMState *state, ASTNode *ast);
static int is_boolean_operator(ASTNode *ast);

static void mark_types_operator(struct HMState *state, ASTNode *ast) {
    ast->left->type = get_new_type(state);
    mark_types(state, ast->left);

    if (is_boolean_operator(ast->condition)) {
        ast->condition->type = TYPE_BOOL;
    } else {
        ast->condition->type = ast->left->type;
    }

    if (ast->right != NULL) {
        ast->right->type = ast->left->type;
        mark_types(state, ast->right);
    }
}

static int is_boolean_operator(ASTNode *ast) {
    switch (ast->op) {
        case OP_EQ:
        case OP_NE:
        case OP_GE:
        case OP_LE:
        case OP_NOT:
        case OP_LT:
        case OP_GT:
            return 1;

        default:
            /* ignore non boolean ops */
            return 0;
    }
}

static void mark_types_node(struct HMState *state, ASTNode *ast) {
    switch (ast->kind) {
        case FUNC_CALL:
            ast->type = get_new_type(state);
            break;

        case ASSIGN_EXPR:
            puts("ASSIGN_EXPR");
            break;

        case OPERATOR:
            mark_types_operator(state, ast);
            puts("OPERATOR");
            break;

        case RETURN_STMT:
            puts("RETURN_STMT");
            break;

        case LOAD_STMT:
            puts("LOAD_STMT");
            break;

        case CONDITIONAL:
            puts("CONDITIONAL");
            break;

        case FUNC_DEF: {
            ASTNode *node = ast->left;
            ast->type = get_new_type(state);
            while (node) {
                node->type = get_new_type(state);
                node = node->sibling;
            }
            mark_types(state, ast->right);
            break;
        }
    }
}

static int mark_types(struct HMState *state, ASTNode *ast) {
    while (ast) {
        mark_types_node(state, ast);
        ast = ast->sibling;
    }
    return 0;
}

int check_types(ASTNode *ast) {
    struct HMState state;
    int exit_code = 0;
    hmstate_init(&state);
    mark_types(&state, ast);
    /*collapse_types(&state);*/
    /*exit_code = check_conflicing_types(&state);*/
    hmstate_free(&state);
    return exit_code;
}
