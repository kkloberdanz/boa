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

#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>

void *iba_malloc(size_t num_bytes);
void iba_free_all(void);
void *iba_realloc(void *ptr, size_t size);

#endif /* MEMORY_H */
