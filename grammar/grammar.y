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

%{
#define YYPARSER

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>

#include "compiler/ast.h"
#include "compiler/typeinfo.h"
#include "util/util.h"

#define YYSTYPE ASTNode *
#define YYERROR_VERBOSE

static int yylex();
void yyerror(const char *msg);
static ASTNode *tree = NULL;
static FILE *source_file = NULL;
char **all_types = NULL;

%}

%define parse.error verbose
%define parse.lac full

%token THEN
%token ENDFILE
%token ERROR
%token IF
%token SLIM_ARROW
%token ELSE
%token ELIF
%token PRINT
%token DEF
%token INT
%token ID
%token INTEGER
%token TYPE
%token ASSIGN
%token EQ
%token NE
%token FAT_ARROW
%token LT
%token GE
%token LE
%token GT
%token PLUS
%token MINUS
%token TIMES
%token OVER
%token PERCENT
%token CARROT
%token LPAREN
%token RPAREN
%token LBRACE
%token RBRACE
%token SEMICOLON
%token COMMENT
%token COLON
%token COMMA
%token QUOTE
%token NEWLINE
%token STRING

%left EQ NE LT GE LE GT
%left MINUS PLUS
%left TIMES OVER PERCENT
%right CARROT

%%
prog        : maybe_newlines
            | maybe_newlines prog
            | stmts                 {debug_puts("prog"); tree = $1;}
            ;

stmts       : stmt                  {debug_puts("stmt"); $$ = $1;}
            | stmt newlines         {debug_puts("stmt"); $$ = $1;}
            | stmts stmt            {
                                        YYSTYPE ast;
                                        debug_puts("stmts");
                                        ast = $1;
                                        if (ast) {
                                            while (ast->sibling) {
                                                ast = ast->sibling;
                                            }
                                            ast->sibling = $2;
                                            $$ = $1;
                                        } else {
                                            $$ = $2;
                                        }
                                    }

            ;

newlines    : NEWLINE
            | NEWLINE newlines
            | SEMICOLON newlines
            | SEMICOLON
            | COMMENT maybe_newlines
            ;

maybe_newlines  : newlines
                |
                ;

stmt        : expr newlines         {$$ = $1;}
            | if_stmt               {$$ = $1;}
            | assign_expr newlines  {$$ = $1;}
            | decl_func             {$$ = $1;}
            | SLIM_ARROW expr maybe_newlines  {$$ = make_return_node($2);}
            ;

decl_func   : id ASSIGN params FAT_ARROW LBRACE newlines
              stmts
              RBRACE newlines       {
                                        debug_puts("decl_func");
                                        $$ = make_function_node($1, $7, $3);
                                    }

            | DEF id LPAREN RPAREN LBRACE newlines
              stmts
              RBRACE newlines       {
                                        debug_puts("decl_func");
                                        $$ = make_function_node($2, $7, NULL);
                                    }
            ;

params      : expr                  {$$ = $1;}
            | expr COMMA params     {
                                        YYSTYPE ast = $3;
                                        if (ast) {
                                            while (ast->sibling) {
                                                ast = ast->sibling;
                                            }
                                            ast->sibling = $1;
                                            $$ = $3;
                                        } else {
                                            $$ = $1;
                                        }
                                    }
            ;

assign_expr : id ASSIGN expr        {
                                        debug_puts("assign_expr");
                                        $$ = make_assign_node(
                                            $1,
                                            $3,
                                            NULL,
                                            all_types);
                                    }
            | id COLON type ASSIGN expr {$$ = make_assign_node(
                                            $1,
                                            $5,
                                            $3,
                                            all_types
                                            );
                                         }
            ;

if_stmt     : IF expr COLON maybe_newlines
                  stmts
              ELSE maybe_newlines
                  stmts             {$$ = make_conditional_node($2, $5, $8);}

expr        : expr PLUS expr        {
                                        debug_puts("making OP_PLUS");
                                        $$ = make_operator_node(
                                            OP_PLUS,
                                            $1,
                                            $3
                                        );
                                    }
            | expr MINUS expr       {
                                        debug_puts("making OP_MINUS");
                                        $$ = make_operator_node(
                                            OP_MINUS,
                                            $1,
                                            $3
                                        );
                                    }
            | expr TIMES expr       {
                                        debug_puts("making OP_TIMES");
                                        $$ = make_operator_node(
                                            OP_TIMES,
                                            $1,
                                            $3
                                        );
                                    }
            | expr OVER expr        {
                                        $$ = make_operator_node(
                                            OP_DIVIDE,
                                            $1,
                                            $3
                                        );
                                    }
            | expr PERCENT expr     {
                                        $$ = make_operator_node(
                                            OP_MOD,
                                            $1,
                                            $3
                                        );
                                    }
            | bool_expr             {$$ = $1;}
            | call_func             {$$ = $1;}
            | LPAREN expr RPAREN    {$$ = $2;}
            | INTEGER               {
                                        $$ = make_literal_node(
                                            make_string(token_string),
                                            AST_INT
                                        );
                                    }
            | STRING                {
                                        $$ = make_literal_node(
                                            make_string(token_string),
                                            AST_STRING
                                        );
                                    }
            | id                    {$$ = make_load_node($1);}
            ;

type        : TYPE                  {
                                        $$ = make_literal_node(
                                            make_string(token_string),
                                            AST_TYPE
                                        );
                                    }
            ;

bool_expr   : expr EQ expr          {$$ = make_operator_node(OP_EQ, $1, $3);}
            | expr LT expr          {$$ = make_operator_node(OP_LT, $1, $3);}
            | expr LE expr          {$$ = make_operator_node(OP_LE, $1, $3);}
            | expr GT expr          {$$ = make_operator_node(OP_GT, $1, $3);}
            | expr GE expr          {$$ = make_operator_node(OP_GE, $1, $3);}
            | expr NE expr          {$$ = make_operator_node(OP_NE, $1, $3);}
            ;

args        : expr                  {$$ = $1;}
            | expr COMMA args       {
                                        YYSTYPE ast = $3;
                                        if (ast) {
                                            while (ast->sibling) {
                                                ast = ast->sibling;
                                            }
                                            ast->sibling = $1;
                                            $$ = $3;
                                        } else {
                                            $$ = $1;
                                        }
                                    }
            ;

call_func   : id LPAREN args RPAREN {
                                        debug_puts("call_func");
                                        $$ = make_func_call_node($1, $3);
                                    }
            | id LPAREN RPAREN      {
                                        debug_puts("call_func");
                                        $$ = make_func_call_node($1, NULL);
                                    }
            ;

id          : ID                    {
                                        $$ = make_id_node(
                                            make_string(token_string)
                                        );
                                    }
            ;

%%

ASTNode *parse(FILE *src_file) {
    int i = 0;
    int num_builtins = 0;
    for (num_builtins = 0; builtin_types[num_builtins]; num_builtins++);
    all_types = malloc(sizeof(char *) * (20 + num_builtins));
    for (i = 0; builtin_types[i] != NULL; i++) {
        all_types[i] = builtin_types[i];
    }
    source_file = src_file;
    yyparse();
    free(all_types);
    return tree;
}

static int yylex(void) {
    int token = get_token(source_file);
#ifdef DEBUG
    printf("token: '%s'\n", token_string);
#endif
    return token;
}
