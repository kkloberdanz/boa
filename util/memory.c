#include "scimath.h"

static MemoryVec memory;

void *boa_malloc(size_t num_bytes) {
    static char is_initialized = 0;
    void *bytes = NULL;
    if (!is_initialized) {
        memory_vec_init(&memory);
        is_initialized = 1;
    }
    bytes = kk_track_malloc(num_bytes, &memory);
    return bytes;
}

void boa_free_all() {
    kk_track_free(&memory);
}
