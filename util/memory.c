/*
 *     This file is part of iba.
 *
 *  iba is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  iba is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with iba.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "scimath.h"
#include "memory.h"

static MemoryVec memory;

void *iba_malloc(size_t num_bytes) {
    static char is_initialized = 0;
    void *bytes = NULL;
    if (!is_initialized) {
        memory_vec_init(&memory);
        is_initialized = 1;
    }
    bytes = kk_track_malloc(num_bytes, &memory);
    return bytes;
}

void *iba_realloc(void *ptr, size_t size) {
    void *bytes = realloc(ptr, size);
    ksm_void_ptr_vector_push(&memory, bytes);
    return bytes;
}

void iba_free_all() {
    kk_track_free(&memory);
}
