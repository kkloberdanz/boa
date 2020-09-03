/*
 *     This file is part of boa.
 *
 *  boa is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  boa is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with boa.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#include "runtime.h"
#include "boaobj.h"

struct BoaObj *println(const struct BoaObj *obj) {
    print(obj);
    printf("\n");
    return create_boa_nil();
}

struct BoaObj *print(const struct BoaObj *obj) {
    switch (obj->type) {
        case BOA_NIL:
            printf("Nil");
            break;

        case BOA_INT:
            printf("%ld", obj->data.i);
            break;

        case BOA_FLOAT:
            printf("%f", obj->data.f);
            break;

        case BOA_STRING:
            puts(obj->data.s);
            break;

        case BOA_BOOL:
            if (obj->data.b) {
                printf("True");
            } else {
                printf("False");
            }
            break;

        case BOA_LIST: {
            struct BoaObj **list = obj->data.l;
            size_t len = obj->len;
            printf("%s", "[");
            if (list && len) {
                size_t i;
                for (i = 0; i < len - 1; i++) {
                    struct BoaObj *node = list[i];
                    print(node);
                    printf(", ");
                }
                print(list[len - 1]);
            }
            printf("%s", "]");
            break;
        }
    }
    return create_boa_nil();
}

struct BoaObj *append(struct BoaObj *list, struct BoaObj *item) {
    if (list->type != BOA_LIST) {
        fprintf(stderr, "append() can only be used on a list");
    }
    list->len++; /* TODO: 2x amortize this */
    list->data.l = realloc(list->data.l, sizeof(*list->data.l) * list->len);
    list->data.l[list->len - 1] = item;
    return list;
}
