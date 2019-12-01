#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

char *make_string(char *str) {
    int len = strlen(str);
    char *buf = malloc((1 + len) * sizeof(char));
    memcpy(buf, str, len);
    return buf;
}

void debug_puts(char *str) {
    UNUSED(str);
#ifdef DEBUG
    puts(str);
#endif
}
