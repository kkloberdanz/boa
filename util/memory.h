#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>

void *boa_malloc(size_t num_bytes);
void boa_free_all();

#endif /* MEMORY_H */
