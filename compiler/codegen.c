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

static void write_start(FILE *output_file) {
    /* TODO: instead of using strings, build boilerplate into
     * an archive (*.a) and link to the generated code */

    fprintf(output_file, "#include <stdio.h>\n");
    fprintf(output_file, "void print(char *str) {\n");
    fprintf(output_file, "    printf(\"%%s\\n\", str);\n");
    fprintf(output_file, "}\n");
    fprintf(output_file, "int main(int argc, char *argv[]) {\n");
}

static void write_end(FILE *output_file) {
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
    fprintf(output_file, "%s(\"%s\");\n", func_name, arg);
}

static void decide_instruction(FILE *output_file, ASTNode *ast) {
    switch (ast->kind) {
        case FUNC_CALL:
            emit_func_call(output_file, ast);
            break;

        case ASSIGN_EXPR:
            /* TODO */
            break;

        default:
            break;
    }
}

static void codegen(FILE *output_file, ASTNode *ast) {
    while (ast) {
        decide_instruction(output_file, ast);
        ast = ast->sibling;
    }
}

int emit(FILE *output_file, ASTNode *ast) {
    write_start(output_file);
    codegen(output_file, ast);
    write_end(output_file);
    return 0;
}
