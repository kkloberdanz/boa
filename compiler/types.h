#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>

enum BuiltinType {
    TYPE_NOT_CHECKED,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
};

struct BoaType {
    int type_id;
    char *repr;
};

int string_repr_to_type_id(char *repr, char *all_types[]);

#endif /* TYPES_H */
