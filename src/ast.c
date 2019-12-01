/*
 *     This file is part of Boa.
 *
 *  Boa is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Boa is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Boa.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "util.h"

char token_string[MAX_TOKEN_SIZE + 1];

ASTNode *make_ast_node(
    ASTkind kind,
    ParseObj *obj,
    Operator op,
    ASTNode *left,
    ASTNode *condition,
    ASTNode *right
) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->kind = kind;
    node->sibling = NULL;
    node->obj = obj;
    node->op = op;
    node->left = left;
    node->condition = condition;
    node->right = right;
    return node;
}

ASTNode *make_leaf_node(ParseObj *obj) {
    ASTNode *node = make_ast_node(LEAF, obj, OP_NIL, NULL, NULL, NULL);
    return node;
}

ASTNode *make_operator_node(Operator op, ASTNode *left, ASTNode *right) {
    ASTNode *node = make_ast_node(OPERATOR, NULL, op, left, NULL, right);
    return node;
}

ASTNode *make_conditional_node(
    ASTNode *condition,
    ASTNode *left,
    ASTNode *right
) {
    ASTNode *node = make_ast_node(
        CONDITIONAL,
        NULL,
        OP_NIL,
        left,
        condition,
        right
    );
    return node;
}

ASTNode *make_assign_node(ASTNode *leaf_obj, ASTNode *right) {
    ParseObj *obj = leaf_obj->obj;
    ASTNode *node = make_ast_node(ASSIGN_EXPR, obj, OP_NIL, NULL, NULL, right);
    return node;
}

ASTNode *make_declare_node(ASTNode *leaf_obj) {
    ParseObj *obj = leaf_obj->obj;
    ASTNode *node = make_ast_node(DECLARE_STMT, obj, OP_NIL, NULL, NULL, NULL);
    return node;
}

ASTNode *make_load_node(ASTNode *leaf_obj) {
    ParseObj *obj = leaf_obj->obj;
    ASTNode *node = make_ast_node(LOAD_STMT, obj, OP_NIL, NULL, NULL, NULL);
    return node;
}

ASTNode *make_function_node(ASTNode *leaf_obj, ASTNode *right) {
    ParseObj *obj = leaf_obj->obj;
    ASTNode *node = make_ast_node(FUNC_DEF, obj, OP_NIL, NULL, NULL, right);
    return node;
}

ASTNode *make_func_call_node(ASTNode *leaf_obj, ASTNode *args) {
    ParseObj *obj = leaf_obj->obj;
    ASTNode *node = make_ast_node(FUNC_CALL, obj, OP_NIL, NULL, NULL, args);
    return node;
}

ParseObj *make_parseobj(char *repr, enum ASTLiteralKind kind) {
    ParseObj *obj = malloc(sizeof(ParseObj));
    obj->repr = repr;
    obj->kind = kind;
    return obj;
}

ASTNode *make_literal_node(char *repr, enum ASTLiteralKind kind) {
    ParseObj *obj = make_parseobj(repr, kind);
    ASTNode *node = make_ast_node(LEAF, obj, OP_NIL, NULL, NULL, NULL);
    return node;
}

ASTNode *make_id_node(char *repr) {
    return make_literal_node(repr, AST_ID);
}
