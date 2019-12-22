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
#include <string.h>

#include "codegen.h"
#include "ast.h"
#include "../util/util.h"

struct CodegenState {
    FILE *outf;
    unsigned long reg;
};

static void codegen_node(struct CodegenState *state, ASTNode *ast);
static void codegen(struct CodegenState *state, ASTNode *ast);

static void write_start(struct CodegenState *state) {
    fprintf(state->outf, "#include <stdio.h>\n");
    fprintf(state->outf, "#include \"../runtime/runtime.h\"\n");
}

static void write_end(struct CodegenState *state) {
    fprintf(state->outf, "return 0;\n");
    fprintf(state->outf, "}\n");
}

static void write_main(struct CodegenState *state) {
    fprintf(state->outf, "int main() {\n");
}

static void emit_func_call(struct CodegenState *state, ASTNode *ast) {
    /*
     * TODO:
     * ast->right contains the args list
     * eval each arg and store in register
     * call function with each arg
     */
    char *func_name = ast->obj->repr;
    char *arg = "";
    char reg_s[255];
    if (ast->right != NULL) {
        codegen(state, ast->right);
        sprintf(reg_s, "r%lu", state->reg);
        arg = reg_s;
    }
    state->reg++;
    fprintf(
        state->outf,
        "const int r%lu = %s(%s);\n",
        state->reg,
        func_name,
        arg
    );
}

static void emit_assign_expr(struct CodegenState *state, ASTNode *ast) {
    char *variable_name = ast->obj->repr;
    codegen_node(state, ast->right);
    fprintf(state->outf, "int %s = r%lu;\n", variable_name, state->reg);
}

static char *get_operator(Operator op) {
    char *op_repr = NULL;
    switch (op) {
        case OP_PLUS:
            op_repr = "+";
            break;

        case OP_TIMES:
            op_repr = "*";
            break;

        case OP_MINUS:
            op_repr = "-";
            break;

        case OP_DIVIDE:
            op_repr = "/";
            break;

        case OP_EQ:
            op_repr = "==";
            break;

        case OP_LT:
            op_repr = "<";
            break;

        case OP_LE:
            op_repr = "<=";
            break;

        case OP_GT:
            op_repr = ">";
            break;

        case OP_GE:
            op_repr = ">=";
            break;

        case OP_NE:
            op_repr = "!=";
            break;

        case OP_NOT:
            op_repr = "!";
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
        operand_1 = ast->left->obj->repr;
    }

    if (ast->right->obj == NULL) {
        emit_operation_expr(state, ast->right);
        sprintf(buf2, "r%lu", state->reg);
        operand_2 = buf2;
    } else {
        operand_2 = ast->right->obj->repr;
    }

    state->reg++;
    fprintf(
        state->outf,
        "const int r%lu = %s %s %s;\n",
        state->reg,
        operand_1,
        operator,
        operand_2
    );
}

static void emit_return_stmt(struct CodegenState *state, ASTNode *ast) {
    codegen_node(state, ast->right);
    fprintf(state->outf, "return r%lu;\n", state->reg);
}

static void emit_load_stmt(struct CodegenState *state, ASTNode *ast) {
    state->reg++;
    fprintf(state->outf, "const int r%lu = %s;\n", state->reg, ast->obj->repr);
}

static void emit_conditional_expr(struct CodegenState *state, ASTNode *ast) {
    codegen(state, ast->condition);
    fprintf(state->outf, "if (r%lu) {\n", state->reg);
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

        /* definitions are handled in first pass, skip for second pass */
        case FUNC_DEF:
            break;
    }
}

static void emit_func_def(struct CodegenState *state, ASTNode *ast) {
    fprintf(state->outf, "int %s() {\n", ast->obj->repr);
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
