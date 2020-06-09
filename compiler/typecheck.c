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

    if (ast->condition) {
        if (is_boolean_operator(ast->condition)) {
            ast->condition->type = TYPE_BOOL;
        } else {
            ast->condition->type = ast->left->type;
        }
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
    if (ast->obj) {
        /*printf("looking at: %s\n", ast->obj->repr);*/
        switch (ast->obj->kind) {
            case AST_STRING:
                debug_puts("STRING");
                ast->type = TYPE_STRING;
                return;

            case AST_INT:
                debug_puts("INT");
                ast->type = TYPE_STRING;
                return;

            case AST_FLOAT:
                debug_puts("FLOAT");
                ast->type = TYPE_FLOAT;
                return;

            case AST_BOOL:
                debug_puts("BOOL");
                ast->type = TYPE_BOOL;
                return;

            default:
                break;
        }
    }

    switch (ast->kind) {
        case FUNC_CALL:
            debug_puts("FUNC_CALL");
            ast->type = get_new_type(state);
            break;

        case ASSIGN_EXPR:
            debug_puts("ASSIGN_EXPR");
            break;

        case OPERATOR:
            debug_puts("OPERATOR");
            mark_types_operator(state, ast);
            break;

        case RETURN_STMT:
            /*
             * TODO: Must track context to ensure all return values are equal and to set the
             * type of the function to the type of the return value
             */
            debug_puts("RETURN_STMT");
            break;

        case LOAD_STMT:
            debug_puts("LOAD_STMT");
            break;

        case CONDITIONAL:
            /*
             * conditional must be a bool
             * left and right sides of condition must be same type
             *
             * TODO: Must track context somehow to ensure that final values of conditions
             * are the same type
             */
            debug_puts("CONDITIONAL");
            ast->type = get_new_type(state);
            ast->condition->type = TYPE_BOOL;
            ast->left->type = get_new_type(state);
            ast->right->type = ast->left->type;
            mark_types(state, ast->left);
            mark_types(state, ast->right);
            mark_types(state, ast->condition);
            break;

        case FUNC_DEF: {
            ASTNode *func_param = ast->left;
            ast->type = get_new_type(state);
            while (func_param) {
                func_param->type = get_new_type(state);
                func_param = func_param->sibling;
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
