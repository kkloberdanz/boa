#include <stdio.h>

#include "runtime.h"
#include "../compiler/util.h"

void print(char *str) {
    printf("%s\n", str);
}

int run(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    return 0;
}
