#!/bin/bash

PROG_NAME=$1

reset && ./compile release && ./boa example/${PROG_NAME}.boa && cat example/*.c && gcc -std=iso9899:1990 -Os -Wall -Werror -Wpedantic -Wextra example/${PROG_NAME}.c && ./a.out
