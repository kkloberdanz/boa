#include <stdio.h>
#include <stdlib.h>

#include "../compiler/typeinfo.h"
#include "memory.h"
#include "set.h"

void set_insert(struct Set *set, TypeId id) {
    while (set) {
        if (id < set->id) {
            if (set->left) {
                set = set->left;
            } else {
                set->left = iba_malloc(sizeof(struct Set));
                set = set->left;
                set->id = id;
                set->right = NULL;
                return;
            }
        } else if (id > set->id) {
            if (set->right) {
                set = set->right;
            } else {
                set->right = iba_malloc(sizeof(struct Set));
                set = set->right;
                set->id = id;
                set->left = NULL;
                return;
            }
        } else {
            return;
        }
    }
}

void set_rec_print(struct Set *set) {
    if (set) {
        set_rec_print(set->left);
        printf("%lu ", set->id);
        set_rec_print(set->right);
    }
}

void set_print(struct Set *set) {
    set_rec_print(set);
    puts("");
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

void set_init(struct Set *set) {
    set->left = NULL;
    set->right = NULL;
    set->id = 0;
}

#ifdef IBA_SET_TEST
int main() {
    size_t i;
    struct Set set;
    set_init(&set);
    int arr[] = {4, 6, 3, 7, 9, 1, 4, 11};
    for (i = 0; i < (sizeof(arr) / sizeof(int)); i++) {
        set_insert(&set, arr[i]);
        set_print(&set);
    }

    set_print(&set);
    for (i = 0; i < 20; i++) {
        if (set_contains(&set, i)) {
            printf("set contains: %d\n", (int)i);
        }
    }

    iba_free_all();
    return 0;
}
#endif /* IBA_SET_TEST */
