/*
 *     This file is part of boa.
 *
 *  boa is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  boa is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with boa.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "typeinfo.h"
#include "../util/memory.h"
#include "../util/util.h"

char token_string[MAX_TOKEN_SIZE + 1];

ASTNode *make_ast_node(
    ASTkind kind,
    ParseObj *obj,
    Operator op,
    ASTNode *left,
    ASTNode *condition,
    ASTNode *right,
    TypeId type
) {
    ASTNode *node = boa_malloc(sizeof(ASTNode));
    node->kind = kind;
    node->sibling = NULL;
    node->obj = obj;
    node->op = op;
    node->left = left;
    node->condition = condition;
    node->right = right;
    node->type = type;
    return node;
}

ASTNode *make_operator_node(Operator op, ASTNode *left, ASTNode *right) {
    ASTNode *node;
    TypeId type;
    type = TYPE_NOT_CHECKED;
    node = make_ast_node(
        OPERATOR,
        NULL,
        op,
        left,
        NULL,
        right,
        type
    );
    return node;
}

ASTNode *make_conditional_node(
    ASTNode *condition,
    ASTNode *left,
    ASTNode *right
) {
    TypeId type;
    ASTNode *node;
    type = TYPE_NOT_CHECKED;
    node = make_ast_node(
        CONDITIONAL,
        NULL,
        OP_NIL,
        left,
        condition,
        right,
        type
    );
    return node;
}

ASTNode *make_assign_node(
    ASTNode *leaf_obj,
    ASTNode *right,
    ASTNode *type_node,
    char **all_types
) {
    ASTNode *node;
    ParseObj *obj = leaf_obj->obj;
    TypeId type = TYPE_NOT_CHECKED;
    if (type_node) {
        ParseObj *type_obj = type_node->obj;
        type = string_repr_to_type_id(type_obj->repr, all_types);
    }

    node = make_ast_node(
        ASSIGN_EXPR,
        obj,
        OP_NIL,
        NULL,
        NULL,
        right,
        type
    );
    return node;
}

ASTNode *make_load_node(ASTNode *leaf_obj) {
    ASTNode *node;
    ParseObj *obj = leaf_obj->obj;
    TypeId type = TYPE_NOT_CHECKED;
    node = make_ast_node(
        LOAD_STMT,
        obj,
        OP_NIL,
        NULL,
        NULL,
        NULL,
        type
    );
    return node;
}

ASTNode *make_function_node(
    ASTNode *func_name,
    ASTNode *func_body,
    ASTNode *params
) {
    ASTNode *node;
    ParseObj *obj = func_name->obj;
    TypeId type;
    type = TYPE_NOT_CHECKED;
    node = make_ast_node(
        FUNC_DEF,
        obj,
        OP_NIL,
        params,
        NULL,
        func_body,
        type
    );
    return node;
}

ASTNode *make_func_call_node(ASTNode *leaf_obj, ASTNode *args) {
    ASTNode *node;
    ParseObj *obj = leaf_obj->obj;
    TypeId type;
    type = TYPE_NOT_CHECKED;
    node = make_ast_node(
        FUNC_CALL,
        obj,
        OP_NIL,
        NULL,
        NULL,
        args,
        type
    );
    return node;
}

ParseObj *make_parseobj(char *repr, enum ASTLiteralKind kind) {
    ParseObj *obj = boa_malloc(sizeof(ParseObj));
    if (obj == NULL) {
        fprintf(stderr, "out of memory");
        exit(EXIT_FAILURE);
    }
    obj->repr = make_string(repr);
    obj->kind = kind;
    return obj;
}

ASTNode *make_literal_node(char *repr, enum ASTLiteralKind kind) {
    ParseObj *obj;
    ASTNode *node;
    TypeId type;
    type = TYPE_NOT_CHECKED;
    switch (kind) {
        case AST_STRING: {
            type = TYPE_STRING;
            string_replace_single_quote_with_double(repr);
            break;
        }

        case AST_INT:
            type = TYPE_INT;
            break;

        case AST_FLOAT:
            type = TYPE_FLOAT;
            break;

        case AST_BOOL:
            type = TYPE_BOOL;
            break;

        case AST_TYPE:
        case AST_ID:
            break;
    }
    obj = make_parseobj(repr, kind);
    node = make_ast_node(
        LOAD_STMT,
        obj,
        OP_NIL,
        NULL,
        NULL,
        NULL,
        type
    );
    return node;
}

ASTNode *make_id_node(char *repr) {
    return make_literal_node(repr, AST_ID);
}

ASTNode *make_return_node(ASTNode *expr_to_return) {
    ASTNode *node;
    TypeId type;
    type = TYPE_NOT_CHECKED;
    node = make_ast_node(
        RETURN_STMT,
        NULL,
        OP_NIL,
        NULL,
        NULL,
        expr_to_return,
        type
    );
    return node;
}

void ast_print(ASTNode *ast) {
    UNUSED(ast);
}
