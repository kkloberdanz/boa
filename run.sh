#!/bin/bash

PROG_NAME=$1

reset && ./compile release && ./boa example/${PROG_NAME}.boa && cat example/*.c && gcc -Os example/${PROG_NAME}.c && ./a.out
