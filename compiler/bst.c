/*
 * Author: Kyle Kloberdanz
 * Project Start Date: 27 Nov 2018
 * License: GNU GPLv3 (see LICENSE.txt)
 *     This file is part of minic.
 *
 *     minic is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     minic is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with minic.  If not, see <https://www.gnu.org/licenses/>.
 * File: bst.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bst.h"

struct BST *bst_new(char *key, int value) {
    struct BST *node = malloc(sizeof(struct BST));
    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

struct BST *bst_insert(struct BST *root, char *key, int value) {
    int comparison;
    struct BST *node = root;
    if (node == NULL) {
        return bst_new(key, value);
    }
_tail_insert:
    comparison = strcmp(node->key, key);
    if (comparison < 0) {
        if (node->right == NULL) {
            node->right = bst_new(key, value);
        } else {
            node = node->right;
            goto _tail_insert;
        }
    } else if (comparison > 0) {
        if (node->left == NULL) {
            node->left = bst_new(key, value);
        } else {
            node = node->left;
            goto _tail_insert;
        }
    } else {
        node->value = value;
    }
    return root;
}

void bst_print_node(struct BST* node) {
    printf("{\"%s\": %d}\n", node->key, node->value);
}

void bst_print(struct BST* node) {
    if (node != NULL) {
        bst_print(node->left);
        bst_print_node(node);
        bst_print(node->right);
    }
}

struct BST *bst_find(struct BST* node, const char *key) {
    int comparison;
    if (node == NULL) {
        return NULL;
    }
_tail_find:
    comparison = strcmp(node->key, key);
    if (comparison < 0) {
        if (node->right == NULL) {
            return NULL;
        } else {
            node = node->right;
            goto _tail_find;
        }
    } else if (comparison > 0) {
        if (node->left == NULL) {
            return NULL;
        } else {
            node = node->left;
            goto _tail_find;
        }
    } else {
        return node;
    }
}

void bst_destroy(struct BST* node) {
    if (node != NULL) {
        free(node->key);
        bst_destroy(node->left);
        bst_destroy(node->right);
        free(node);
    }
}
