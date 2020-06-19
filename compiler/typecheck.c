#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "typecheck.h"
#include "typeinfo.h"
#include "../util/util.h"
#include "../util/memory.h"

/*
 * TODO:
 *     Revise typechecking, instead of current algorithm, use pointers.
 *     1) If TypeId *type is NULL
 *         - Allocate memory for type, assign it to TYPE_NOT_CHECKED
 *     2) If encountered concrete type and type is not NULL and *type == TYPE_NOT_CHECKED
 *         - Assign *type to concrete type
 *     3) If encountered concrete type and type is not NULL and *type != TYPE_NOT_CHECKED
 *         - Type error, conflicting types
 *     4) If type not known and type is not NULL and *type == TYPE_NOT_CHECKED
 *         - Point type to the address of an equivalent type
 */

static int mark_types(ASTNode *ast, TypeId context);
static char is_boolean_operator(ASTNode *ast);

static TypeId *type_rules(TypeId *ptr, TypeId proposed_type) {
    if (ptr == NULL) {
        return new_type(proposed_type);
    } else if (ptr && *ptr == TYPE_NOT_CHECKED) {
        /* TODO: deallocate? */
        return new_type(proposed_type);
    } else if (ptr && *ptr != TYPE_NOT_CHECKED) {
        fprintf(
            stderr,
            "conflicint types: %s and %s\n",
            builtin_types[*ptr],
            builtin_types[proposed_type]
        );
        exit(EXIT_FAILURE);
    } else {
        fprintf(
            stderr,
            "conflicint types: %s and %s\n",
            builtin_types[*ptr],
            builtin_types[proposed_type]
        );
        exit(EXIT_FAILURE);
    }
}

static void mark_types_operator(ASTNode *ast, TypeId context) {
    /*ast->left->type = get_new_type(state);*/
    mark_types(ast->left, context);

    if (ast->condition) {
        if (is_boolean_operator(ast->condition)) {
            ast->condition->type = type_rules(ast->condition->type, TYPE_BOOL);
        } else {
            ast->condition->type = ast->left->type;
        }
    }

    if (ast->right != NULL) {
        ast->right->type = ast->left->type;
        mark_types(ast->right, context);
    }
}

static char is_boolean_operator(ASTNode *ast) {
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

static void mark_types_node(ASTNode *ast, TypeId context) {
    if (ast->obj) {
        /*printf("looking at: %s\n", ast->obj->repr);*/
        switch (ast->obj->kind) {
            case AST_STRING:
                debug_puts("STRING");
                ast->type = type_rules(ast->type, TYPE_STRING);
                return;

            case AST_INT:
                debug_puts("INT");
                ast->type = type_rules(ast->type, TYPE_INT);
                return;

            case AST_FLOAT:
                debug_puts("FLOAT");
                ast->type = type_rules(ast->type, TYPE_FLOAT);
                return;

            case AST_BOOL:
                debug_puts("BOOL");
                ast->type = type_rules(ast->type, TYPE_BOOL);
                return;

            default:
                break;
        }
    }

    switch (ast->kind) {
        case FUNC_CALL:
            debug_puts("FUNC_CALL");

            /* TODO: make a more efficient way to lookup type for builtin functions */
            if (strcmp(ast->obj->repr, "printd")) {
                ast->type = type_rules(ast->type, TYPE_INT);
            }
            ast->right->type = ast->type;
            /*context = ast->type;*/
            break;

        case ASSIGN_EXPR:
            debug_puts("ASSIGN_EXPR");
            break;

        case OPERATOR:
            debug_puts("OPERATOR");
            mark_types_operator(ast, context);
            break;

        case RETURN_STMT:
            /*
             * TODO: Must track context to ensure all return values are equal and to set the
             * type of the function to the type of the return value
             */
            debug_puts("RETURN_STMT");
            mark_types(ast->right, context);
            /*add_equivalent_type(state, context, ast->right->type);*/
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
            /*
            ast->type = get_new_type(state);
            add_equivalent_type(state, ast->type, context);
            context = ast->type;
            ast->condition->type = TYPE_BOOL;
            ast->left->type = get_new_type(state);
            ast->right->type = ast->left->type;
            mark_types(state, ast->left, context);
            mark_types(state, ast->right, context);
            mark_types(state, ast->condition, context);
            */
            break;

        case FUNC_DEF: {
            /*
            ASTNode *func_param = ast->left;
            ast->type = get_new_type(state);
            context = ast->type;
            while (func_param) {
                func_param->type = get_new_type(state);
                func_param = func_param->sibling;
            }
            mark_types(state, ast->right, context);
            */
            break;
        }
    }
}

static int mark_types(ASTNode *ast, TypeId context) {
    while (ast) {
        mark_types_node(ast, context);
        ast = ast->sibling;
    }
    return 0;
}

/**
 * Hack to force everything to be an int for testing
 */
static int assign_types(ASTNode *ast) {
    while (ast) {
        ast->type = new_type(TYPE_INT);
        assign_types(ast->right);
        assign_types(ast->left);
        assign_types(ast->condition);
        ast = ast->sibling;
    }
    return 0;
}

int check_types(ASTNode *ast) {
    int exit_code = 0;
    TypeId context = TYPE_NOT_CHECKED;
    mark_types(ast, context);
    assign_types(ast);
    return exit_code;
}
