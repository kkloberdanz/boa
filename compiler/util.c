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

void string_replace_single_quote_with_double(char *str) {
    size_t i;
    size_t len = strlen(str);
    for (i = 0; i < len; i++) {
        if ((i == 0) || (i == (len - 1))){
            if (str[i] == '\'') {
                str[i] = '"';
            }
        }
    }
}
