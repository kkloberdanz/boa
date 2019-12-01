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

#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdbool.h>

enum { MAX_TOKEN_SIZE=100 };
extern char token_string[MAX_TOKEN_SIZE + 1];

void greet(void);

/* enums */
typedef enum {
    VOID_TYPE,  /* voids must be resolved during type deduction */
    NUMBER_TYPE,
    REAL_TYPE,
    BOOL_TYPE,
    STRING_TYPE
} MinicType;


typedef enum {
    CONDITIONAL,
    OPERATOR,
    LEAF,
    ASSIGN_EXPR,
    DECLARE_STMT,
    LOAD_STMT,
    FUNC_DEF,
    FUNC_CALL
} ASTkind;


typedef enum {
    OP_NIL,
    OP_PLUS,
    OP_MINUS,
    OP_TIMES,
    OP_DIVIDE,
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
    AST_FLOAT
};

/* structs */
typedef struct MinicObject {
    MinicType type;
    union {
        char *number_value;
        char *real_value;
        bool bool_value;
        char *string_value;
        char *symbol;
    } value;
} MinicObject;


typedef struct ASTNode {
    ASTkind kind;
    MinicObject *obj;
    Operator op;
    struct ASTNode *left;
    struct ASTNode *condition;
    struct ASTNode *right;
    struct ASTNode *sibling;
} ASTNode;

ASTNode *make_assign_node(ASTNode *leaf_obj, ASTNode *right);
ASTNode *make_declare_node(ASTNode *leaf_obj);
ASTNode *make_load_node(ASTNode *leaf_obj);
ASTNode *make_function_node(ASTNode *leaf_obj, ASTNode *right);
ASTNode *make_func_call_node(ASTNode *leaf_obj, ASTNode *args);
char *make_string(char *str);

ASTNode *make_ast_node(ASTkind, /* base constructor */
                       MinicObject *,
                       Operator,
                       ASTNode *,
                       ASTNode *,
                       ASTNode *);

ASTNode *make_leaf_node(MinicObject *); /* just holds minic object */

ASTNode *make_operator_node(Operator,  /* holds operator and child items */
                            ASTNode *, /* to operate on */
                            ASTNode *);

ASTNode *make_conditional_node(ASTNode *left,
                               ASTNode *condition,
                               ASTNode *right);

ASTNode *make_id_node(char *str);

ASTNode *make_literal_node(char *str, enum ASTLiteralKind);

int get_token(FILE *source_file);

#endif /* AST_H */
