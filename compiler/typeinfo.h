#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>

typedef unsigned long TypeId;

extern char *builtin_types[];

enum BuiltinType {
    TYPE_NOT_CHECKED,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_NIL,
    TYPE_LIST,
    TYPE_LASTTYPE /* always keep this last */
};

struct boaType {
    char *repr;
    TypeId id;
};

TypeId string_repr_to_type_id(char *repr, char *all_types[]);

const char *boa_type_to_c_type(TypeId type_id);

TypeId *new_type(TypeId t);

#endif /* TYPES_H */
