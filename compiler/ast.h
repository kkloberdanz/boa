/*
 *     This file is part of iba.
 *
 *  iba is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  iba is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with iba.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>

#include "typeinfo.h"

enum { MAX_TOKEN_SIZE = 100 };
extern char token_string[MAX_TOKEN_SIZE + 1];

/* enums */
typedef enum {
    CONDITIONAL,
    OPERATOR,
    ASSIGN_EXPR,
    FUNC_DEF,
    LOAD_STMT,
    RETURN_STMT,
    FUNC_CALL
} ASTkind;

typedef enum {
    OP_NIL,
    OP_PLUS,
    OP_MINUS,
    OP_TIMES,
    OP_DIVIDE,
    OP_MOD,
    OP_EQ,
    OP_LT,
    OP_LE,
    OP_GT,
    OP_GE,
    OP_NE,
    OP_NOT
} Operator;

enum ASTLiteralKind {
    AST_STRING,
    AST_INT,
    AST_FLOAT,
    AST_BOOL,
    AST_ID,
    AST_TYPE
};

/* structs */
typedef struct ParseObj {
    char *repr;
    enum ASTLiteralKind kind;
    int _pad; /* unused 4 byte padding for alignment */
} ParseObj;

typedef struct ASTNode {
    ParseObj *obj;
    struct ASTNode *left;
    struct ASTNode *condition;
    struct ASTNode *right;
    struct ASTNode *sibling;
    ASTkind kind;
    Operator op;
    TypeId type;
} ASTNode;

ParseObj *make_parseobj(char *repr, enum ASTLiteralKind kind);

ASTNode *make_assign_node(
    ASTNode *leaf_obj,
    ASTNode *right,
    ASTNode *type_node,
    char **all_types
);

ASTNode *make_declare_node(ASTNode *leaf_obj);
ASTNode *make_load_node(ASTNode *leaf_obj);

ASTNode *make_function_node(
    ASTNode *leaf_obj,
    ASTNode *right,
    ASTNode *params
);

ASTNode *make_func_call_node(ASTNode *leaf_obj, ASTNode *args);

ASTNode *make_ast_node(
    ASTkind kind,
    ParseObj *obj,
    Operator op,
    ASTNode *left,
    ASTNode *condition,
    ASTNode *right,
    TypeId type
);

ASTNode *make_leaf_node(ParseObj *); /* just holds minic object */

ASTNode *make_operator_node(
    Operator,  /* holds operator and child items */
    ASTNode *, /* to operate on */
    ASTNode *
);

ASTNode *make_conditional_node(
    ASTNode *left,
    ASTNode *condition,
    ASTNode *right
);

ASTNode *make_id_node(char *repr);

ASTNode *make_literal_node(char *repr, enum ASTLiteralKind kind);

int get_token(FILE *source_file);

ASTNode *parse(FILE *src_file);

ASTNode *make_return_node(ASTNode *expr_to_return);

#endif /* AST_H */
