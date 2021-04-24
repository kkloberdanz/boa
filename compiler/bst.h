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
 * File: bst.h
 */

#ifndef BST_H
#define BST_H

struct BST {
    char *key;
    int value;
    struct BST *left;
    struct BST *right;
};

struct BST *bst_new(char *key, int value);
struct BST *bst_insert(struct BST *node, char *key, int value);
void bst_print(struct BST *node);
void bst_print_node(struct BST *node);
struct BST *bst_find(struct BST *node, const char *key);
void bst_destroy(struct BST *bst);

#endif
