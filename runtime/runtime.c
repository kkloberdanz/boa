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
#include <string.h>

#include "runtime.h"
#include "boaobj.h"

struct BoaObj *println(struct BoaObj *args) {
    print(args);
    printf("\n");
    return create_boa_nil();
}

struct BoaObj *print(struct BoaObj *args) {
    const struct BoaObj *obj = args->data.l[0];
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
            struct BoaObj args_list;
            struct BoaObj *data[1];
            args_list.data.l = data;
            args_list.len = 1;
            args_list.cap = 1;
            args_list.type = BOA_LIST;
            printf("%s", "[");
            if (list && len) {
                size_t i;
                for (i = 0; i < len - 1; i++) {
                    struct BoaObj *node = list[i];
                    args_list.data.l[0] = node;

                    print(&args_list);
                    printf(", ");
                }
                args_list.data.l[0] = list[len - 1];
                print(&args_list);
            }
            printf("%s", "]");
            break;
        }
        case BOA_FUNC:
            puts("<function>");
            break;
    }
    return create_boa_nil();
}

struct BoaObj *append(struct BoaObj *args) {
    struct BoaObj *list = args->data.l[0];
    struct BoaObj *item = args->data.l[1];
    if (list->type != BOA_LIST) {
        fprintf(stderr, "append() can only be used on a list");
        exit(EXIT_FAILURE);
    }
    list->len++;
    if (!list->cap) {
        list->cap = list->len;
        list->data.l = malloc(sizeof(*list->data.l) * list->len);
    } else if (list->len > list->cap) {
        list->cap = 2 * list->len;
        list->data.l = realloc(
            list->data.l,
            sizeof(*list->data.l) * list->cap
        );
    }
    list->data.l[list->len - 1] = item;
    return list;
}

struct BoaObj *map(struct BoaObj *args) {
    struct BoaObj *func = args->data.l[0];
    struct BoaObj *list = args->data.l[1];
    struct BoaObj args_list;
    struct BoaObj *data[1];
    size_t len = list->len;
    size_t i = 0;
    struct BoaObj *new_obj;
    if (list->type != BOA_LIST) {
        fprintf(stderr, "map() can only be used on a list");
        exit(EXIT_FAILURE);
    }
    if (func->type != BOA_FUNC) {
        fprintf(stderr, "map() can only be used on a list");
        exit(EXIT_FAILURE);
    }
    new_obj = malloc(sizeof(struct BoaObj));
    new_obj->type = BOA_LIST;
    new_obj->len = len;
    new_obj->cap = len;
    new_obj->data.l = malloc(len * sizeof(struct BoaObj *));

    args_list.data.l = data;
    args_list.type = BOA_LIST;
    args_list.len = 1;
    args_list.cap = 1;
    for (i = 0; i < len; i++) {
        struct BoaObj *node = list->data.l[i];
        args_list.data.l[0] = node;
        new_obj->data.l[i] = func->data.fn(&args_list);
    }
    return new_obj;
}

struct BoaObj *reduce(struct BoaObj *args) {
    struct BoaObj *func = args->data.l[0];
    struct BoaObj *list = args->data.l[1];
    struct BoaObj args_list;
    struct BoaObj *data[2];
    size_t len = list->len;
    size_t i = 0;
    struct BoaObj *new_obj;

    args_list.data.l = data;

    if (list->type != BOA_LIST) {
        fprintf(stderr, "reduce() can only be used on a list\n");
        exit(EXIT_FAILURE);
    }
    if (func->type != BOA_FUNC) {
        fprintf(stderr, "reduce() can only be used on a list\n");
        exit(EXIT_FAILURE);
    }
    if (len == 0) {
        fprintf(stderr, "cannot reduce on empty list\n");
        exit(EXIT_FAILURE);
    }
    new_obj = malloc(sizeof(struct BoaObj));
    new_obj->type = list->data.l[0]->type;
    new_obj->data = list->data.l[0]->data;
    for (i = 1; i < len; i++) {
        struct BoaObj *node = list->data.l[i];
        /*
         * For this to work, function arguments must be passed in as a list
         * Each C function will recieve 1 and only 1 argument, which will
         * be the arguments list
         */
        args_list.data.l[0] = new_obj;
        args_list.data.l[1] = node;
        new_obj = func->data.fn(&args_list);
    }
    return new_obj;
}
