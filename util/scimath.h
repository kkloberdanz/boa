/******************************************************************************
 * Copyright 2019 Kyle Kloberdanz
 *****************************************************************************/

#ifndef SCIMATH_H
#define SCIMATH_H

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <pthread.h>

#define ksm_UNUSED(X) (void)(X)

#ifndef ksm_DERIV_H_CONST
#define ksm_DERIV_H_CONST 0.0001
#endif

#ifndef ksm_VECTOR_START_CAPACITY
#define ksm_VECTOR_START_CAPACITY 4
#endif

#ifndef ksm_VECTOR_GROW_RATE
#define ksm_VECTOR_GROW_RATE 2
#endif

#ifndef kk_BTREE_SIZE
#define kk_BTREE_SIZE 2
#endif

/******************************************************************************
 * By default use libc malloc, realloc, and free.
 * These can be overridden with custom implementations if desired.
 *****************************************************************************/
#ifndef ksm_MALLOC
#define ksm_MALLOC malloc
#endif

#ifndef ksm_FREE
#define ksm_FREE free
#endif

#ifndef ksm_REALLOC
#define ksm_REALLOC realloc
#endif

/******************************************************************************
 * Enums
 *****************************************************************************/
enum ksm_ErrorCode {
    ksm_NO_ERROR,
    ksm_OUT_OF_MEMORY
};

enum ksm_MaybeKind {
    ksm_SOME,
    ksm_NONE
};

/******************************************************************************
 * Structs
 *****************************************************************************/
struct LinkedList {
    void *node;
    struct LinkedList *next;
};

struct MemoryPoolNode {
    char *memory;
    struct MemoryPoolNode *next;
    size_t index;
    size_t capacity;
    size_t id;
};

struct Arena {
    pthread_mutex_t _mtx;
    struct MemoryPoolNode *_pool;
    struct MemoryPoolNode *_full_pool;
};

struct BTreeNode {
    char *key;
    double value;
};

struct BTreeBlock {
    struct BTreeNode data[kk_BTREE_SIZE];
    struct BTreeNode *ptrs[kk_BTREE_SIZE + 1];
    size_t index;
};

struct BTree {
    int (*compare_keys)(void *, void *);
    size_t size;
    struct BTreeBlock *block;
};

/******************************************************************************
 * Macros
 *****************************************************************************/
#define ksm_TIMEIT(stmt, time_buffer)                                         \
    do {                                                                      \
        struct timeval _ksm_start, _ksm_finish;                               \
        gettimeofday(&_ksm_start, NULL);                                      \
        {                                                                     \
            stmt;                                                             \
        }                                                                     \
        gettimeofday(&_ksm_finish, NULL);                                     \
        *time_buffer =                                                        \
            (1000000 * _ksm_finish.tv_sec + _ksm_finish.tv_usec) -            \
            (1000000 * _ksm_start.tv_sec + _ksm_start.tv_usec);               \
    } while (0)


/******************************************************************************
 * Generic Template Macros
 * Usage:
 * Call the *_HEADER macro in a .h file, and include this in any source that
 * will utilize the given macro
 * Then call the *_IMPL in a .c file to include the source implementation
 *****************************************************************************/
#define ksm_GENERIC_MAX( TYPE )                                               \
TYPE ksm_##TYPE##_MAX( TYPE a, TYPE b ) {                                     \
    return (a > b) ? a : b;                                                   \
}

#define ksm_GENERIC_MAYBE_HEADER( TYPE )                                      \
    struct ksm_##TYPE##_Maybe {                                               \
        enum ksm_MaybeKind kind;                                              \
        TYPE value;                                                           \
    };

#define ksm_GENERIC_VECTOR_HEADER( TYPE )                                     \
    struct ksm_##TYPE##_Vector {                                              \
        size_t size;                                                          \
        size_t capacity;                                                      \
        TYPE *data;                                                           \
    };                                                                        \
                                                                              \
    enum ksm_ErrorCode ksm_##TYPE##_vector_init(                              \
        struct ksm_##TYPE##_Vector *v);                                       \
                                                                              \
    enum ksm_ErrorCode ksm_##TYPE##_vector_push(                              \
        struct ksm_##TYPE##_Vector *v, TYPE value);                           \
                                                                              \
    enum ksm_ErrorCode ksm_##TYPE##_vector_free(                              \
        struct ksm_##TYPE##_Vector *v);                                       \
                                                                              \
    enum ksm_MaybeKind ksm_##TYPE##_vector_pop(                               \
        struct ksm_##TYPE##_Vector *v);                                       \

#define ksm_GENERIC_VECTOR_IMPL( TYPE )                                       \
    enum ksm_ErrorCode ksm_##TYPE##_vector_init(                              \
        struct ksm_##TYPE##_Vector *v                                         \
    ) {                                                                       \
        TYPE *data = ksm_MALLOC(sizeof(TYPE) * ksm_VECTOR_START_CAPACITY);    \
        v->size = 0;                                                          \
        v->capacity = ksm_VECTOR_START_CAPACITY;                              \
        v->data = data;                                                       \
        if (data == NULL) {                                                   \
            return ksm_OUT_OF_MEMORY;                                         \
        } else {                                                              \
            return ksm_NO_ERROR;                                              \
        }                                                                     \
    }                                                                         \
                                                                              \
    enum ksm_ErrorCode ksm_##TYPE##_vector_push(                              \
        struct ksm_##TYPE##_Vector *v,                                        \
        TYPE value                                                            \
    ) {                                                                       \
        if (v->size < v->capacity) {                                          \
            v->data[v->size] = value;                                         \
            v->size++;                                                        \
        } else {                                                              \
            const size_t new_capacity = ksm_VECTOR_GROW_RATE * v->capacity;   \
            v->capacity = new_capacity;                                       \
            v->data = ksm_REALLOC(v->data, v->capacity * sizeof(TYPE));       \
            if (v->data != NULL) {                                            \
                v->data[v->size] = value;                                     \
                v->size++;                                                    \
            } else {                                                          \
                return ksm_OUT_OF_MEMORY;                                     \
            }                                                                 \
        }                                                                     \
        return ksm_NO_ERROR;                                                  \
    }                                                                         \
                                                                              \
    enum ksm_ErrorCode ksm_##TYPE##_vector_free(                              \
        struct ksm_##TYPE##_Vector *v                                         \
    ) {                                                                       \
        ksm_FREE(v->data);                                                    \
        return ksm_NO_ERROR;                                                  \
    }                                                                         \
                                                                              \
    enum ksm_MaybeKind ksm_##TYPE##_vector_pop(                               \
        struct ksm_##TYPE##_Vector *v                                         \
    ) {                                                                       \
        if (v->size > 0) {                                                    \
            const size_t new_capacity = v->capacity / ksm_VECTOR_GROW_RATE;   \
            v->size--;                                                        \
            if (v->size < new_capacity) {                                     \
                v->data = ksm_REALLOC(v->data, new_capacity * sizeof(TYPE));  \
                v->capacity = new_capacity;                                   \
            }                                                                 \
            return ksm_SOME;                                                  \
        } else {                                                              \
            return ksm_NONE;                                                  \
        }                                                                     \
    }

typedef void * void_ptr;
ksm_GENERIC_VECTOR_HEADER(void_ptr)
typedef struct ksm_void_ptr_Vector MemoryVec;

/******************************************************************************
 * Function prototypes
 *****************************************************************************/
double ksm_first_deriv(double (*f)(double), double x);
double ksm_second_deriv(double (*f)(double), double x);
void ksm_map(double (*f)(double), double *dst, const double *src, size_t size);
void ksm_vector_f64_sqrt(double *dst, const double *src, size_t size);
void kk_arena_init(struct Arena *arena);
void *kk_arena_alloc(size_t size, struct Arena *arena);
void kk_arena_free_all(struct Arena *arena);
void *kk_track_malloc(size_t size, struct ksm_void_ptr_Vector *vec);
void kk_track_free(struct ksm_void_ptr_Vector *vec);

#define memory_vec_init(X) ksm_void_ptr_vector_init(X)

#endif /* SCIMATH_H */
