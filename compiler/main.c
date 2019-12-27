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
#include <stdbool.h>
#include <string.h>

#include "ast.h"
#include "../util/util.h"
#include "../util/memory.h"
#include "codegen.h"
#include "typecheck.h"

static bool is_boa_src_file(char *filename) {
    unsigned long len = strlen(filename) - 1;
    if (len < 5) {
        return false;
    }
    return filename[len - 3] == '.' \
        && filename[len - 2] == 'b' \
        && filename[len - 1] == 'o' \
        && filename[len] == 'a';
}

static int compile_boa(char *source_filename, char *output_filename) {
    FILE *source_file = NULL;
    FILE *output = NULL;
    unsigned long len = strlen(source_filename) - 1;
    int exit_code = 1;
    ASTNode *tree = NULL;

    if (!is_boa_src_file(source_filename)) {
        fprintf(stderr, "not a Boa source file: %s\n", source_filename);
        return 2;
    }
    source_file = fopen(source_filename, "r");
    if (source_file == NULL) {
        fprintf(stderr, "no such file:%s\n", source_filename);
        return 3;
    }
    tree = parse(source_file);
    fclose(source_file);

    if (tree == NULL) {
        fprintf(stderr, "%s\n", "failed to parse input");
        return 4;
    }

    exit_code = typecheck(tree);
    if (exit_code) {
        return exit_code;
    }

    memcpy(output_filename, source_filename, len);
    output_filename[len - 2] = 'c';
    output_filename[len - 1] = '\0';
    output = fopen(output_filename, "w");

    if (output == NULL) {
        fprintf(stderr, "%s\n", "failed to open output file");
        return 5;
    }

    exit_code = emit(output, tree);
    if (fclose(output) != 0) {
        fprintf(stderr, "%s\n", "failed to close output file");
        return 6;
    }
    boa_free_all();
    return exit_code;
}

int main(int argc, char **argv) {
    int exit_code = 1;
    if (argc != 2) {
        fprintf(stderr, "usage: %s FILENAME\n", argv[0]);
        return 1;
    } else {
        char c_filename[1000];
        char *source_filename = argv[1];
        exit_code = compile_boa(source_filename, c_filename);
        /* TODO: compile and run *.c program */
        return exit_code;
    }
}
