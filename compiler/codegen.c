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

static void decide_instruction(FILE *output_file, ASTNode *ast) {
    switch (ast->kind) {
        case FUNC_CALL: {
            /*
             * TODO:
             * ast->right contains the args list
             * eval each arg and store in register
             * call function with each arg
             */
            char *func_name = ast->obj->repr;
            char *arg = ast->right->obj->repr;
            fprintf(output_file, "%s(\"%s\");\n", func_name, arg);
            break;
        }

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
