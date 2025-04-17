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

#include "scimath.h"
#include "memory.h"

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

void *boa_realloc(void *ptr, size_t size) {
    void *bytes = realloc(ptr, size);
    ksm_void_ptr_vector_push(&memory, bytes);
    return bytes;
}

void boa_free_all(void) {
    kk_track_free(&memory);
}
