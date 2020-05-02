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
#include <libgen.h>
#include <getopt.h>

#include "ast.h"
#include "../util/util.h"
#include "../util/memory.h"
#include "codegen.h"
#include "typecheck.h"

static const char *program_name = "iba";

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

static void c_filename_to_exe_filename(char **filename) {
    char *ptr;

    *filename = basename(*filename);
    ptr = *filename;
    while (*ptr) {
        if ((*ptr == '.') &&
            ((*(ptr + 1)) == 'c') &&
            ((*(ptr + 2)) == '\0')
        ) {
            *ptr = '\0';
            return;
        } else {
            ptr++;
        }
    }
}

static int compile_iba(const char *source_filename, char **output_filename) {
    FILE *source_file = NULL;
    FILE *output = NULL;
    unsigned long len = strlen(source_filename) - 1;
    int error_code = 1;
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

    error_code = typecheck(tree);
    if (error_code) {
        return error_code;
    }

    *output_filename = make_string(source_filename);
    (*output_filename)[len - 2] = 'c';
    (*output_filename)[len - 1] = '\0';

    output = fopen(*output_filename, "w");
    if (output == NULL) {
        perror("failed to open output file");
        return 5;
    }

    error_code = emit(output, tree);
    if (fclose(output) != 0) {
        perror("failed to close output file");
        return 6;
    }
    return error_code;
}

static int compile_c(const char *c_filename, const char *exe_filename) {
    int error_code = 0;
    const char *iba_cc = getenv("IBA_CC");
    char *buf;

    if (!iba_cc) {
        const char *fmt = "%s -Os -fPIC -o %s %s libccruntime.a";
        iba_cc = "cc";

        buf = iba_malloc(
            1 +
            strlen(fmt) +
            strlen(c_filename) +
            strlen(exe_filename) +
            strlen(iba_cc)
        );
        fprintf(
            stderr,
            "\nenvironment variable IBA_CC not set, defaulting to cc\n\n"
        );
        sprintf(
            buf,
            fmt,
            iba_cc,
            exe_filename,
            c_filename
        );

    } else if (!strcmp(iba_cc, "tcc")) {
        const char *fmt = "%s -Os -fPIC -o %s %s libruntime.a";
        buf = iba_malloc(
            1 +
            strlen(fmt) +
            strlen(c_filename) +
            strlen(exe_filename) +
            strlen(iba_cc)
        );
        sprintf(
            buf,
            fmt,
            iba_cc,
            exe_filename,
            c_filename
        );

    } else {
        const char *fmt = "%s -Os -fPIC -static -o %s %s libruntime.a";
        buf = iba_malloc(
            1 +
            strlen(fmt) +
            strlen(c_filename) +
            strlen(exe_filename) +
            strlen(iba_cc)
        );
        sprintf(
            buf,
            fmt,
            iba_cc,
            exe_filename,
            c_filename
        );
    }

    error_code = system(buf);
    if (error_code) {
        const char *fmt = "%s -Os -fPIC -o %s %s libccruntime.a";

        fprintf(stderr, "failed to compile '%s'\n", exe_filename);
        fprintf(stderr, "are you using the right C compiler?\n");
        fprintf(stderr, "tried using: '%s'\n", iba_cc);
        fprintf(stderr, "\nfailed default compilation, using fallback\n");
        iba_cc = "cc";

        buf = iba_malloc(
            1 +
            strlen(fmt) +
            strlen(c_filename) +
            strlen(exe_filename) +
            strlen(iba_cc)
        );
        iba_cc = "cc";
        fprintf(
            stderr,
            "\nenvironment variable IBA_CC not set, defaulting to cc\n\n"
        );
        sprintf(
            buf,
            fmt,
            iba_cc,
            exe_filename,
            c_filename
        );

        error_code = system(buf);
        if (error_code) {
            fprintf(stderr, "failed to compile '%s'\n", exe_filename);
            fprintf(stderr, "are you using the right C compiler?\n");
            fprintf(stderr, "tried using: '%s'\n", iba_cc);
            return error_code;
        }
    }
    return error_code;
}

static int run_program(const char *exe_filename) {
    char *buf;
    const char *fmt = "./%s";
    int error_code = 0;

    buf = iba_malloc(1 + strlen(fmt) + strlen(exe_filename));
    sprintf(buf, fmt, exe_filename);
    error_code = system(buf);
    if (error_code) {
        fprintf(stderr, "failed to run: '%s'\n", exe_filename);
        return error_code;
    }
    return error_code;
}

static void print_usage(const char *prg_name) {
    fprintf(stderr, "\n\n%s: [-b] [-o outfile] infile\n\n", prg_name);

    fprintf(stderr, "        -b    Build only, don't run the iba program\n\n"
                    "        -o outfile\n"
                    "              Instead of the default output filename,\n"
                    "              write binary to outfile\n\n"
    );
}

static int run(int argc, char **argv) {
    int error_code = 1;
    char *c_filename = NULL;
    const char *source_filename;
    int opt;
    char *exe_filename = NULL;
    char run = 0;
    char build = 0;

    while ((opt = getopt(argc, argv, "rbho:")) != -1) {
        switch (opt) {
            case 'r':
                run = 1;
                break;

            case 'h':
                print_usage(program_name);
                exit(EXIT_SUCCESS);

            case 'b':
                build = 1;
                break;

            case 'o':
                exe_filename = optarg;
                build = 1;
                break;

            default:
                print_usage(program_name);
                return EXIT_FAILURE;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "no iba source file provided\n");
        print_usage(program_name);
        return EXIT_FAILURE;
    } else {
        source_filename = argv[optind];
    }

    error_code = compile_iba(source_filename, &c_filename);
    if (error_code) {
        fprintf(stderr, "failed to compile '%s'\n", source_filename);
        return error_code;
    }

    if (!exe_filename) {
        exe_filename = make_string(c_filename);
        c_filename_to_exe_filename(&exe_filename);
    }

    error_code = compile_c(c_filename, exe_filename);
    remove(c_filename);
    if (error_code) {
        return error_code;
    }

    if (run) {
        run_program(exe_filename);
        if (!build) {
            remove(exe_filename);
        }
    }
    return error_code;
}

int main(int argc, char **argv) {
    int error_code = run(argc, argv);
    iba_free_all();
    return error_code;
}
