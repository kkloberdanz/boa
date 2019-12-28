#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>

extern char *builtin_types[];

enum BuiltinType {
    TYPE_NOT_CHECKED,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
};

struct BoaType {
    int id;
    char *repr;
};

int string_repr_to_type_id(char *repr, char *all_types[]);

char *boa_type_to_c_type(int type_id);

#endif /* TYPES_H */
