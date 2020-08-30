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

#include <stdio.h>

#include "runtime.h"
#include "boaobj.h"

struct BoaObj *print(const struct BoaObj *obj) {
    switch (obj->type) {
        case BOA_NIL:
            puts("Nil");
            break;

        case BOA_INT:
            printf("%ld\n", obj->data.i);
            break;

        case BOA_FLOAT:
            printf("%f\n", obj->data.f);
            break;

        case BOA_STRING:
            printf("%s\n", obj->data.s);
            break;

        case BOA_LIST:
            puts("[not yet implemented]");
            break;
    }
    return NULL;
}
