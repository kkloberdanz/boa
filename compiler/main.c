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
#include "codegen.h"

bool is_boa_src_file(char *filename) {
    int len = strlen(filename) - 1;
    if (len < 5) {
        return false;
    }
    return filename[len - 3] == '.' \
           && filename[len - 2] == 'b' \
           && filename[len - 1] == 'o' \
           && filename[len] == 'a';
}

int main(int argc, char **argv) {
    char *output_filename = NULL;
    char *source_filename = NULL;
    FILE *output;
    FILE *source_file;
    int exit_code;
    int len = 0;
    ASTNode *tree = NULL;

    if (argc != 2) {
        fprintf(stderr, "usage: %s FILENAME\n", argv[0]);
        return 1;
    } else {
        source_filename = argv[1];
        len = strlen(source_filename) - 1;
        if (!is_boa_src_file(source_filename)) {
            fprintf(stderr, "not a Boa source file: %s\n", source_filename);
            exit(EXIT_FAILURE);
        }
        source_file = fopen(source_filename, "r");
        if (source_file == NULL) {
            fprintf(stderr, "no such file:%s\n", source_filename);
            exit(EXIT_FAILURE);
        }
        tree = parse(source_file);
        fclose(source_file);

        if (tree == NULL) {
            fprintf(stderr, "%s\n", "failed to parse input");
            exit(EXIT_FAILURE);
        }

        output_filename = make_string(source_filename);
        output_filename[len - 2] = 'c';
        output_filename[len - 1] = '\0';
        output = fopen(output_filename, "w");
        free(output_filename);

        if (output == NULL) {
            fprintf(stderr, "%s\n", "failed to open output file");
            exit(EXIT_FAILURE);
        }

        exit_code = emit(output, tree);
        if (fclose(output) != 0) {
            fprintf(stderr, "%s\n", "failed to close output file");
            exit(EXIT_FAILURE);
        }
        return exit_code;
    }
}
