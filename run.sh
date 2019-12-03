#!/bin/bash

reset && ./compile release && ./boa example/ex1.boa && cat example/*.c && gcc example/ex1.c && ./a.out
