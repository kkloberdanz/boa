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

#include <stdio.h>

#include "runtime.h"

int printd(int num) {
    printf("%d\n", num);
    return 0;
}

int print2d(int n1, int n2) {
    printf("%d %d\n", n1, n2);
    return 0;
}

int print(char *str) {
    printf("%s\n", str);
    return 0;
}
