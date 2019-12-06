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
#include "util.h"

static void codegen_node(FILE *output_file, ASTNode *ast);
static unsigned long reg = 0;

static void write_start(FILE *output_file) {
    /* TODO: instead of using strings, build boilerplate into
     * an archive (*.a) and link to the generated code */
    fprintf(output_file, "#include <stdio.h>\n");
    fprintf(output_file, "void print(char *str) {\n");
    fprintf(output_file, "    printf(\"%%s\\n\", str);\n");
    fprintf(output_file, "}\n");
    fprintf(output_file, "void printd(int n) {\n");
    fprintf(output_file, "    printf(\"%%d\\n\", n);\n");
    fprintf(output_file, "}\n");
    fprintf(output_file, "int main() {\n");
}

static void write_end(FILE *output_file) {
    fprintf(output_file, "return 0;\n");
    fprintf(output_file, "}\n");
}

static void emit_func_call(FILE *output_file, ASTNode *ast) {
    /*
     * TODO:
     * ast->right contains the args list
     * eval each arg and store in register
     * call function with each arg
     */
    char *func_name = ast->obj->repr;
    char *arg = ast->right->obj->repr;
    fprintf(output_file, "%s(%s);\n", func_name, arg);
}

static void emit_assign_expr(FILE *output_file, ASTNode *ast) {
    char *variable_name = ast->obj->repr;
    codegen_node(output_file, ast->right);
    fprintf(output_file, "int %s = r%lu;\n", variable_name, reg);
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

static void emit_operation_expr(FILE *output_file, ASTNode *ast) {
    char buf1[100];
    char buf2[100];
    char *operand_1;
    char *operand_2;
    const char *operator = get_operator(ast->op);

    if (ast->left->obj == NULL) {
        emit_operation_expr(output_file, ast->left);
        sprintf(buf1, "r%lu", reg);
        operand_1 = buf1;
    } else {
        operand_1 = ast->left->obj->repr;
    }

    if (ast->right->obj == NULL) {
        emit_operation_expr(output_file, ast->right);
        sprintf(buf2, "r%lu", reg);
        operand_2 = buf2;
    } else {
        operand_2 = ast->right->obj->repr;
    }

    reg++;
    fprintf(
        output_file,
        "const int r%lu = %s %s %s;\n",
        reg,
        operand_1,
        operator,
        operand_2
    );
}

static void codegen_node(FILE *output_file, ASTNode *ast) {
    switch (ast->kind) {
        case FUNC_CALL:
            emit_func_call(output_file, ast);
            break;

        case ASSIGN_EXPR:
            /* TODO */
            emit_assign_expr(output_file, ast);
            break;

        case OPERATOR:
            emit_operation_expr(output_file, ast);
            break;

        default:
            break;
    }
}

static void codegen(FILE *output_file, ASTNode *ast) {
    while (ast) {
        codegen_node(output_file, ast);
        ast = ast->sibling;
    }
}

int emit(FILE *output_file, ASTNode *ast) {
    write_start(output_file);
    codegen(output_file, ast);
    write_end(output_file);
    return 0;
}
