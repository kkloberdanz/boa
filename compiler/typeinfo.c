#include <stdio.h>
#include <string.h>

#include "typeinfo.h"

char *builtin_types[] = {
    "Void",
    "Int",
    "Float",
    "String",
    "Bool",
    NULL
};

static char *c_types[] = {
    "void",
    "long",
    "double",
    "char *",
    "char",
    NULL
};

TypeId string_repr_to_type_id(char *repr, char *all_types[]) {
    TypeId i;
    for (i = 0; all_types[i] != NULL; i++) {
        if (strcmp(repr, all_types[i]) == 0) {
            return i;
        }
    }
    /* TODO: make this an error condition */
    fprintf(stderr, "type: '%s' unkown\n", repr);
    return TYPE_NOT_CHECKED;
}

char *iba_type_to_c_type(TypeId type_id) {
    return c_types[type_id];
}
