#include <stdio.h>
#include <stdlib.h>

#include "../compiler/typeinfo.h"
#include "set.h"
#include "vec.h"

/*
 * TODO: Make this a red-black tree
 */

struct Set *set_new(TypeId id) {
    struct Set *set = malloc(sizeof(*set));
    set->right = NULL;
    set->left = NULL;
    set->id = id;
    return set;
}

TypeId *set_smallest(struct Set *set) {
    if (set) {
        return &set->id;
    } else {
        return NULL;
    }
}

struct Set *set_insert(struct Set *root, TypeId id) {
    struct Set *node = root;
    if (node == NULL) {
        return set_new(id);
    }
_tail_insert:
    if (node->id < id) {
        if (node->right == NULL) {
            node->right = set_new(id);
        } else {
            node = node->right;
            goto _tail_insert;
        }
    } else if (node->id > id) {
        if (node->left == NULL) {
            node->left = set_new(id);
        } else {
            node = node->left;
            goto _tail_insert;
        }
    } else {
        /* done */
    }
    return root;
}

void set_print(struct Set *set) {
    size_t i;
    struct Vec *v = set_to_vec(set);
    putchar('{');
    if (v) {
        for (i = 0; i < v->last; i++) {
            if (i == v->last - 1) {
                printf("%lu", v->data[i]);
            } else {
                printf("%lu, ", v->data[i]);
            }
        }
    }
    puts("}");
    vec_free(v);
}

int set_contains(struct Set *set, TypeId id) {
    while (set) {
        if (id < set->id) {
            set = set->left;
        } else if (id > set->id) {
            set = set->right;
        } else {
            return 1;
        }
    }
    return 0;
}

void set_free(struct Set *set) {
    if (set) {
        set_free(set->left);
        set_free(set->right);
        free(set);
    }
}

static void set_rec_to_vec(struct Set *set, struct Vec *vec) {
    if (set) {
        set_rec_to_vec(set->left, vec);
        vec_push(vec, set->id);
        set_rec_to_vec(set->right, vec);
    }
}

struct Vec *set_to_vec(struct Set *set) {
    if (set) {
        struct Vec *vec = vec_new();
        set_rec_to_vec(set, vec);
        return vec;
    } else {
        return NULL;
    }
}

#ifdef IBA_SET_TEST
int main(void) {
    size_t i;
    int arr[] = {0, 1, 4, 6, 3, 7, 9, 1, 4, 11, 2};
    struct Set *set = NULL;

    for (i = 0; i < (sizeof(arr) / sizeof(int)); i++) {
        fprintf(stderr, "inserting: %d\n", arr[i]);
        set = set_insert(set, arr[i]);
    }

    set_print(set);
    for (i = 0; i < 20; i++) {
        if (set_contains(set, i)) {
            fprintf(stderr, "set contains: %d\n", (int)i);
        }
    }

    struct Vec *ids = set_to_vec(set);
    for (i = 0; i < ids->last; i++) {
        printf("id = %lu\n", ids->data[i]);
    }
    vec_free(ids);
    set_free(set);
    return 0;
}
#endif /* IBA_SET_TEST */
