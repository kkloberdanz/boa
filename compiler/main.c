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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "ast.h"
#include "../util/util.h"
#include "../util/memory.h"
#include "codegen.h"
#include "typecheck.h"

static bool is_iba_src_file(const char *filename) {
    unsigned long len = strlen(filename) - 1;
    if (len < 5) {
        return false;
    }
    return filename[len - 3] == '.' \
        && filename[len - 2] == 'i' \
        && filename[len - 1] == 'b' \
        && filename[len] == 'a';
}

static void c_filename_to_exe_filename(char *filename) {
    while (*filename) {
        if ((*filename == '.') &&
            ((*(filename + 1)) == 'c') &&
            ((*(filename + 2)) == '\0')
        ) {
            *filename = '\0';
            return;
        } else {
            filename++;
        }
    }
}

static int compile_iba(const char *source_filename, char *output_filename) {
    FILE *source_file = NULL;
    FILE *output = NULL;
    unsigned long len = strlen(source_filename) - 1;
    int exit_code = 1;
    ASTNode *tree = NULL;

    if (!is_iba_src_file(source_filename)) {
        fprintf(stderr, "not a iba source file: %s\n", source_filename);
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

    memcpy(output_filename, source_filename, len + 1);
    output_filename[len - 2] = 'c';
    output_filename[len - 1] = '\0';

    output = fopen(output_filename, "w");
    if (output == NULL) {
        perror("failed to open output file");
        return 5;
    }

    exit_code = emit(output, tree);
    if (fclose(output) != 0) {
        perror("failed to close output file");
        return 6;
    }
    iba_free_all();
    return exit_code;
}

static int compile_c(const char *c_filename, char *exe_filename) {
    int exit_code = 0;
    const char *iba_cc = getenv("IBA_CC");
    char buf[1500];

    strcpy(exe_filename, c_filename);
    c_filename_to_exe_filename(exe_filename);
    if (!iba_cc) {
        iba_cc = "cc";
        fprintf(
            stderr,
            "\nenvironment variable IBA_CC not set, defaulting to cc\n\n"
        );
        sprintf(
            buf,
            "%s -fPIC -o %s %s libccruntime.a",
            iba_cc,
            exe_filename,
            c_filename
        );

    } else if(!strcmp(iba_cc, "tcc")) {
        sprintf(
            buf,
            "%s -fPIC -o %s %s libruntime.a",
            iba_cc,
            exe_filename,
            c_filename
        );

    } else {
        sprintf(
            buf,
            "%s -fPIC -static -o %s %s libruntime.a",
            iba_cc,
            exe_filename,
            c_filename
        );
    }

    exit_code = system(buf);
    if (exit_code) {
        fprintf(stderr, "failed to compile '%s'\n", exe_filename);
        fprintf(stderr, "are you using the right C compiler?\n");
        fprintf(stderr, "tried using: '%s'\n", iba_cc);
        return exit_code;
    }
    return exit_code;
}

static int run_program(const char *exe_filename) {
    char buf[1500];
    int exit_code = 0;

    sprintf(buf, "./%s", exe_filename);
    exit_code = system(buf);
    if (exit_code) {
        perror("failed to run");
        return exit_code;
    }
    return exit_code;
}

int main(int argc, char **argv) {
    int exit_code = 1;
    if (argc != 2) {
        fprintf(stderr, "usage: %s FILENAME\n", argv[0]);
        return 1;
    } else {
        char c_filename[500];
        char exe_filename[500];
        const char *source_filename = argv[1];

        exit_code = compile_iba(source_filename, c_filename);
        if (exit_code) {
            fprintf(stderr, "failed to compile '%s'\n", source_filename);
            return exit_code;
        }

        exit_code = compile_c(c_filename, exe_filename);
        if (exit_code) {
            return exit_code;
        }

        run_program(exe_filename);
        return exit_code;
    }
}
