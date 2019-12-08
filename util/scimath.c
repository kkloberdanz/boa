/******************************************************************************
 * Copyright 2019 Kyle Kloberdanz
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#include "scimath.h"

#define MAX(A, B) ((A) > (B)) ? (A) : (B)
#define PAGE_SIZE 4096

ksm_GENERIC_VECTOR_IMPL(void_ptr)

double ksm_first_deriv(double (*f)(double), double x) {
    return (f(x + ksm_DERIV_H_CONST) - f(x - ksm_DERIV_H_CONST)) /
           (2 * ksm_DERIV_H_CONST);
}

double ksm_second_deriv(double (*f)(double), double x) {
    return (f(x + ksm_DERIV_H_CONST) - 2 * f(x) + f(x - ksm_DERIV_H_CONST)) /
           (ksm_DERIV_H_CONST * ksm_DERIV_H_CONST);
}

static struct MemoryPoolNode *kk_do_malloc(size_t size) {
    size_t capacity = MAX(size, PAGE_SIZE);
    void *memory = malloc(capacity);
    struct MemoryPoolNode *pool = malloc(sizeof(struct MemoryPoolNode));
    pool->memory = memory;
    pool->next = NULL;
    pool->index = size;
    pool->capacity = capacity;
    return pool;
}

void kk_arena_init(struct Arena *arena) {
    arena->_pool = NULL;
    arena->_full_pool = NULL;
}

void *kk_arena_alloc(size_t size, struct Arena *arena) {
start_alloc:
    if (arena->_pool == NULL) {
        /* first allocation */
        struct MemoryPoolNode *pool = kk_do_malloc(size);
        arena->_pool = pool;
        return pool->memory;
    } else {
        struct MemoryPoolNode *pool;
        struct MemoryPoolNode *prev = NULL;
        struct MemoryPoolNode *full_pool = NULL;
        for (pool = arena->_pool; pool != NULL; pool = pool->next) {
            size_t bytes_left = pool->capacity - pool->index;

            if (bytes_left < 10) {
                /* remove full pool from active pools list */
                if (prev == NULL) {
                    arena->_pool = pool->next;
                } else {
                    prev->next = pool->next;
                }

                /* move full pool to the _full_pool list */
                full_pool = arena->_full_pool;
                if (full_pool == NULL) {
                    arena->_full_pool = pool;
                } else {
                    arena->_full_pool = pool;
                    pool->next = full_pool;
                }

                goto start_alloc;

            } else if (size <= bytes_left) {
                /* has available memory in existing pool */
                size_t index = pool->index;
                pool->index += size;
                return pool->memory + index;
            }

            if (pool) {
                prev = pool;
            }
        }
        /* needs to add new pool */
        pool = kk_do_malloc(size);
        prev->next = pool;
        return pool->memory;
    }
}

static void free_pools(struct MemoryPoolNode *pool) {
    struct MemoryPoolNode *head;
    while (pool) {
        head = pool->next;
        free(pool->memory);
        free(pool);
        pool = head;
    }
}

void kk_arena_free_all(struct Arena *arena) {
    free_pools(arena->_pool);
    free_pools(arena->_full_pool);
}

void *kk_track_malloc(size_t size, struct ksm_void_ptr_Vector *vec) {
    void *ptr = malloc(size);
    if (ptr != NULL) {
        ksm_void_ptr_vector_push(vec, ptr);
    }
    return ptr;
}

void kk_track_free(struct ksm_void_ptr_Vector *vec) {
    size_t i;
    for (i = 0; i < vec->size; i++) {
        free(vec->data[i]);
    }
    ksm_void_ptr_vector_free(vec);
}
