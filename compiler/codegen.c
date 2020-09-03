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
#include <string.h>

#include "codegen.h"
#include "typeinfo.h"
#include "ast.h"
#include "../util/util.h"

struct CodegenState {
    FILE *outf;
    unsigned long reg;
};

static void codegen_node(struct CodegenState *state, ASTNode *ast);
static void codegen(struct CodegenState *state, ASTNode *ast);
static void emit_load_stmt(struct CodegenState *state, ASTNode *ast);

static void write_start(struct CodegenState *state) {
    fprintf(state->outf, "#include <stdio.h>\n");
    fprintf(state->outf, "#include <stdlib.h>\n");
    fprintf(state->outf, "#include \"../runtime/runtime.h\"\n");
    fprintf(state->outf, "#include \"../runtime/boaobj.h\"\n");
}

static void write_end(struct CodegenState *state) {
    fprintf(state->outf, "  return 0;\n");
    fprintf(state->outf, "}\n");
}

static void write_main(struct CodegenState *state) {
    fprintf(state->outf, "int main(int argc, char **argv) {\n");
    fprintf(state->outf, "  gc_init();\n");
}

static void emit_list_literal(struct CodegenState *state, ASTNode *ast) {
    size_t len = 0;
    size_t i;
    unsigned long argument_regs[20];

    if (ast) {
        ASTNode *args_list = ast->left;
        /* TODO: clean this up by adding code into boaobj.c */
        /* solve each argument expression, and store the result in
         * argument_regs */
        for (len = 0; args_list != NULL; len++) {
            codegen_node(state, args_list);
            argument_regs[len] = state->reg;
            args_list = args_list->sibling;
        }

        state->reg++;
        fprintf(
            state->outf,
            "struct BoaObj *r%ld = malloc(%lu);\n",
            state->reg,
            sizeof(struct BoaObj *)
        );
        fprintf(
            state->outf,
            "r%ld->type = BOA_LIST;\n",
            state->reg
        );
        fprintf(
            state->outf,
            "r%ld->len = %lu;\n",
            state->reg,
            len
        );
        fprintf(
            state->outf,
            "r%ld->data.l = malloc(%lu);\n",
            state->reg,
            len * sizeof(struct BoaObj *)
        );

        for (i = 0; i < len; i++) {
            fprintf(
                state->outf,
                "r%ld->data.l[%lu] = r%ld;\n",
                state->reg,
                i,
                argument_regs[len - i - 1]
            );
        }
    }
}

static void emit_func_call(struct CodegenState *state, ASTNode *ast) {
    char *func_name = ast->obj->repr;
    unsigned long argument_regs[20];
    struct ASTNode *args_list = ast->right;
    char args_str[255] = {0};
    int num_args = 0;
    int i = 0;
    size_t len = 0;

    /* solve each argument expression, and store the result in
     * argument_regs */
    for (num_args = 0; args_list != NULL; num_args++) {
        codegen_node(state, args_list);
        argument_regs[num_args] = state->reg;
        args_list = args_list->sibling;
    }

    /* argument_regs contains the result of each argument expr.
     * pass each of the results to the function when calling it */
    i = num_args;
    while (i --> 0) {
        if (i > 0) {
            sprintf(args_str + len, "r%lu, ", argument_regs[i]);
        } else {
            sprintf(args_str + len, "r%lu", argument_regs[i]);
        }
        len = strlen(args_str);
    }

    state->reg++;
    fprintf(
        state->outf,
        "struct BoaObj *r%lu = %s(%s);\n",
        state->reg,
        func_name,
        args_str
    );
}

static void emit_assign_expr(struct CodegenState *state, ASTNode *ast) {
    char *variable_name = ast->obj->repr;
    codegen_node(state, ast->right);
    fprintf(
        state->outf,
        "struct BoaObj *%s = r%lu;\n",
        variable_name,
        state->reg
    );
}

static char *get_operator(Operator op) {
    char *op_repr = NULL;
    switch (op) {
        case OP_PLUS:
            op_repr = "perform_add";
            break;

        case OP_TIMES:
            op_repr = "perform_mul";
            break;

        case OP_MINUS:
            op_repr = "perform_sub";
            break;

        case OP_DIVIDE:
            op_repr = "perform_div";
            break;

        case OP_MOD:
            op_repr = "perform_mod";
            break;

        case OP_EQ:
            op_repr = "perform_equ";
            break;

        case OP_LT:
            op_repr = "perform_lt";
            break;

        case OP_LE:
            op_repr = "perform_lte";
            break;

        case OP_GT:
            op_repr = "perform_gt";
            break;

        case OP_GE:
            op_repr = "perform_gte";
            break;

        case OP_NE:
            op_repr = "perform_ne";
            break;

        case OP_NOT:
            op_repr = "perform_not";
            break;

        case OP_NIL:
            op_repr = NULL;
            break;
    }
    return op_repr;
}

static void emit_operation_expr(struct CodegenState *state, ASTNode *ast) {
    char buf1[100];
    char buf2[100];
    char *operand_1;
    char *operand_2;
    const char *operator = get_operator(ast->op);

    if (ast->left->obj == NULL) {
        emit_operation_expr(state, ast->left);
        sprintf(buf1, "r%lu", state->reg);
        operand_1 = buf1;
    } else {
        emit_load_stmt(state, ast->left);
        sprintf(buf1, "r%lu", state->reg);
        operand_1 = buf1;
    }

    if (ast->right->obj == NULL) {
        emit_operation_expr(state, ast->right);
        sprintf(buf2, "r%lu", state->reg);
        operand_2 = buf2;
    } else {
        emit_load_stmt(state, ast->right);
        sprintf(buf2, "r%lu", state->reg);
        operand_2 = buf2;
    }

    state->reg++;
    fprintf(
        state->outf,
        "struct BoaObj *r%lu = %s(%s, %s);\n",
        state->reg,
        operator,
        operand_1,
        operand_2
    );
}

static void emit_return_stmt(struct CodegenState *state, ASTNode *ast) {
    codegen_node(state, ast->right);
    fprintf(state->outf, "return r%lu;\n", state->reg);
}

static void emit_load_stmt(struct CodegenState *state, ASTNode *ast) {
    char *load_func = NULL;
    switch (ast->obj->kind) {
        case AST_ID:
            load_func = "";
            break;

        case AST_INT:
            load_func = "create_boa_int";
            break;

        case AST_FLOAT:
            load_func = "create_boa_float";
            break;

        case AST_STRING:
            load_func = "create_boa_string";
            break;

        case AST_LIST:
            load_func = "create_boa_list";
            break;

        /* TODO: add all literal types */
        case AST_BOOL: /* TODO: make this work */
        case AST_TYPE:
            fprintf(stderr, "literal type not yet supported\n");
            break;
    }

    state->reg++;
    fprintf(
        state->outf,
        "struct BoaObj *r%lu = %s(%s);\n",
        state->reg,
        load_func,
        ast->obj->repr
    );
}

static void emit_conditional_expr(struct CodegenState *state, ASTNode *ast) {
    codegen(state, ast->condition);
    fprintf(state->outf, "if (r%lu->data.b) {\n", state->reg);
    codegen(state, ast->left);
    fprintf(state->outf, "}\n");
    if (ast->right != NULL) {
        fprintf(state->outf, "else {\n");
        codegen(state, ast->right);
        fprintf(state->outf, "}\n");
    }
}

static void codegen_node(struct CodegenState *state, ASTNode *ast) {
    switch (ast->kind) {
        case FUNC_CALL:
            emit_func_call(state, ast);
            break;

        case ASSIGN_EXPR:
            emit_assign_expr(state, ast);
            break;

        case OPERATOR:
            emit_operation_expr(state, ast);
            break;

        case RETURN_STMT:
            emit_return_stmt(state, ast);
            break;

        case LOAD_STMT:
            emit_load_stmt(state, ast);
            break;

        case CONDITIONAL:
            emit_conditional_expr(state, ast);
            break;

        case LIST_LITERAL:
            emit_list_literal(state, ast);
            break;

        /* definitions are handled in first pass, skip for second pass */
        case FUNC_DEF:
            break;
    }
}

static void emit_func_def(struct CodegenState *state, ASTNode *ast) {
    ASTNode *params = ast->left;
    if (params == NULL) {
        fprintf(
            state->outf,
            "static struct BoaObj *%s() {\n",
            ast->obj->repr
        );
    } else {
        char params_str[255];
        size_t len = 0;
        size_t num_params = 0;
        size_t i = 0;
        ASTNode *params_arr[20] = {NULL};
        while (params) {
            params_arr[num_params] = params;
            params = params->sibling;
            num_params++;
        }

        i = num_params;
        while (i --> 0) {
            sprintf(
                params_str + len,
                "struct BoaObj *%s, ",
                params_arr[i]->obj->repr
            );
            len = strlen(params_str);
        }
        params_str[len - 2] = '\0';
        fprintf(
            state->outf,
            "static struct BoaObj *%s(%s) {\n",
            ast->obj->repr,
            params_str
        );
    }
    codegen(state, ast->right);
    fprintf(state->outf, "}\n");
}

static void codegen_defs_node(struct CodegenState *state, ASTNode *ast) {
    switch (ast->kind) {
        case FUNC_DEF:
            emit_func_def(state, ast);
            break;

        /* only handle definitions in first pass */
        case FUNC_CALL:
        case LIST_LITERAL:
        case ASSIGN_EXPR:
        case OPERATOR:
        case CONDITIONAL:
        case LOAD_STMT:
        case RETURN_STMT:
            break;
    }
}

static void codegen_defs(struct CodegenState *state, ASTNode *ast) {
    while (ast) {
        codegen_defs_node(state, ast);
        ast = ast->sibling;
    }
}

static void codegen(struct CodegenState *state, ASTNode *ast) {
    while (ast) {
        codegen_node(state, ast);
        ast = ast->sibling;
    }
}

int emit(FILE *output_file, ASTNode *ast) {
    struct CodegenState state;
    state.outf = output_file;
    state.reg = 0;
    write_start(&state);
    codegen_defs(&state, ast);
    write_main(&state);
    codegen(&state, ast);
    write_end(&state);
    return 0;
}
