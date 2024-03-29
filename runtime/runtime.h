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

#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdio.h>

#include "boaobj.h"

struct BoaObj *println(struct BoaObj *args);
struct BoaObj *print(struct BoaObj *args);
struct BoaObj *len(struct BoaObj *args);
struct BoaObj *append(struct BoaObj *args);
struct BoaObj *map(struct BoaObj *args);
struct BoaObj *reduce(struct BoaObj *args);

#endif /* RUNTIME_H */
