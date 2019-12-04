#!/bin/bash

PROG_NAME=$1
MODE=$2

reset && ./compile $MODE && ./boa example/${PROG_NAME}.boa && cat example/*.c && gcc -std=iso9899:1990 -Os -Wall -Werror -Wpedantic -Wno-error=unused-variable -Wextra example/${PROG_NAME}.c && ./a.out
