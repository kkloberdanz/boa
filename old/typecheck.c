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
 *
 *     Add type map with identifier mapping to a concrete type
 */

static int mark_types(ASTNode *ast, TypeId *context);
static char is_boolean_operator(ASTNode *ast);

/*
 * TODO:
 *     how to handle NULL type nodes? They should each point to the same
 *     new address
 */
static void set_equiv_types(TypeId *t1, TypeId *t2) {
    if (t1 && !t2) {
        t2 = t1;
    } else if (!t1 && t2) {
        t1 = t2;
    } else if (t1 && t2 && *t1 == *t2) {
        /* do nothing */
    } else if (t1 && t2 && *t1 == TYPE_NOT_CHECKED) {
        *t1 = *t2;
    } else if (t2 && t1 && *t2 == TYPE_NOT_CHECKED) {
        *t2 = *t1;
    } else if (t1 && t2) {
        fprintf(
            stderr,
            "conflicing types: %s and %s\n",
            builtin_types[*t1],
            builtin_types[*t2]
        );
        exit(EXIT_FAILURE);
    } else {
        fprintf(
            stderr,
            "internal compiler error, type nodes are both NULL\n"
        );
    }
}

static TypeId *type_rules(TypeId *ptr, TypeId proposed_type) {
    if (!ptr) {
        return new_type(proposed_type);
    } else if (ptr && *ptr == TYPE_NOT_CHECKED) {
        *ptr = proposed_type;
        return ptr;
    } else if (ptr && *ptr == proposed_type) {
        return ptr;
    } else {
        fprintf(
            stderr,
            "conflicing types: %s and %s\n",
            builtin_types[*ptr],
            builtin_types[proposed_type]
        );
        exit(EXIT_FAILURE);
    }
}

static void mark_types_operator(ASTNode *ast, TypeId *context) {
    mark_types(ast->left, context);

    if (ast->condition) {
        if (is_boolean_operator(ast->condition)) {
            ast->condition->type = type_rules(ast->condition->type, TYPE_BOOL);
        } else {
            set_equiv_types(ast->condition->type, ast->left->type);
        }
    }

    if (ast->right != NULL) {
        set_equiv_types(ast->right->type, ast->left->type);
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

static void mark_types_node(ASTNode *ast, TypeId *context) {
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

            /*
             * TODO:
             *     make a more efficient way to lookup
             *     type for builtin functions
             */
            if (strcmp(ast->obj->repr, "printd")) {
                ast->type = type_rules(ast->type, TYPE_INT);
            }
            set_equiv_types(ast->right->type, ast->type);
            context = ast->type;
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
             * TODO:
             *     Must track context to ensure all return values are equal
             *     and to set the type of the function to the type of the
             *     return value
             */
            debug_puts("RETURN_STMT");
            mark_types(ast->right, context);
            set_equiv_types(ast->right->type, context);
            break;

        case LOAD_STMT:
            debug_puts("LOAD_STMT");
            break;

        case CONDITIONAL:
            /*
             * conditional must be a bool
             * left and right sides of condition must be same type
             *
             * TODO: Must track context somehow to ensure that final
             * values of conditions are the same type
             */
            debug_puts("CONDITIONAL");
            set_equiv_types(ast->type, context);
            ast->condition->type = type_rules(ast->condition->type, TYPE_BOOL);
            set_equiv_types(ast->right->type, ast->left->type);
            mark_types(ast->left, context);
            mark_types(ast->right, context);
            mark_types(ast->condition, context);
            break;

        case FUNC_DEF: {
            ASTNode *func_param = ast->left;

            /* TODO: lookup type in map */
            ast->type = new_type(TYPE_NOT_CHECKED);
            context = ast->type;
            while (func_param) {
                /* TODO: param same type as argument */
                func_param->type = new_type(TYPE_NOT_CHECKED);
                func_param = func_param->sibling;
            }
            mark_types(ast->right, context);
            break;
        }
    }
}

static int mark_types(ASTNode *ast, TypeId *context) {
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
        if (!ast->type) {
            ast->type = new_type(TYPE_INT);
        } else {
            *ast->type = TYPE_INT;
        }
        assign_types(ast->right);
        assign_types(ast->left);
        assign_types(ast->condition);
        ast = ast->sibling;
    }
    return 0;
}

int check_types(ASTNode *ast) {
    int exit_code = 0;
    TypeId *context = NULL;
    mark_types(ast, context);
    assign_types(ast);
    return exit_code;
}
