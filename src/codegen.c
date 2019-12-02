#include <stdio.h>

#include "codegen.h"
#include "ast.h"
#include "util.h"

static void write_start(FILE *output_file) {
    fprintf(output_file, "#include <stdio.h>\n");
    fprintf(output_file, "void print(char *str) {\n");
    fprintf(output_file, "    printf(\"%%s\\n\", str);\n");
    fprintf(output_file, "}\n");
    fprintf(output_file, "int main(int argc, char *argv[]) {\n");
}

static void write_end(FILE *output_file) {
    fprintf(output_file, "}\n");
}

static void codegen(FILE *output_file, ASTNode *ast) {
    printf("node id: %zu\n", ast->id);

    switch (ast->kind) {
        case FUNC_CALL: {
            /*
             * TODO:
             * ast->right contains the args list
             * eval each arg and store in register
             * call function with each arg
             */
            char *s1 = ast->obj->repr;
            char *s2 = ast->right->obj->repr;
            fprintf(output_file, "%s(\"%s\");\n", s1, s2);
            puts("emiting func_call");
            break;
        }

        default:
            puts("default");
            break;
    }
}

int emit(FILE *output_file, ASTNode *ast) {
    UNUSED(output_file);
    UNUSED(ast);

    write_start(output_file);
    codegen(output_file, ast);
    write_end(output_file);
    return 0;
}
