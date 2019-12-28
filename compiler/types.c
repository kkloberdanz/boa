#include <stdio.h>
#include <string.h>

#include "types.h"

int string_repr_to_type_id(char *repr, char *all_types[]) {
    unsigned int i;
    for (i = 0; all_types[i] != NULL; i++) {
        if (strcmp(repr, all_types[i]) == 0) {
            return i;
        }
    }
    /* TODO: make this an error condition */
    fprintf(stderr, "type: '%s' unkown\n", repr);
    return TYPE_NOT_CHECKED;
}
