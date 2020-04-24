#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>

typedef long TypeId;

extern char *builtin_types[];

enum BuiltinType {
    TYPE_NOT_CHECKED,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_LASTTYPE /* always keep this last */
};

struct BoaType {
    char *repr;
    TypeId id;
};

int string_repr_to_type_id(char *repr, char *all_types[]);

char *iba_type_to_c_type(TypeId type_id);

#endif /* TYPES_H */
