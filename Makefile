STD ?= c89
WARN_FLAGS ?= -Wall -Wextra -Wpedantic -Wno-padded
INCLD ?= -I.
CFLAGS ?= -fPIC

.PHONY: release
release: dynamic

.PHONY: static
static: export OPTIM_FLAGS ?= -Os -static -s
static: export CC := musl-gcc
static: export BOA_CC := musl-gcc
static: build

.PHONY: clang-warn-everything
clang-warn-everything: export WARN_FLAGS := \
	-Weverything \
	-Wno-format-nonliteral \
	-Wno-padded \
	-Wno-float-equal
clang-warn-everything: export CC := clang
clang-warn-everything: export BOA_CC := clang
clang-warn-everything: build

.PHONY: dynamic
dynamic: export OPTIM_FLAGS ?= -Os
dynamic: export CC ?= cc
dynamic: export BOA_CC ?= cc
dynamic: build

.PHONY: debug
debug: export OPTIM_FLAGS ?= -O0 -ggdb3 -Werror -DDEBUG
debug: export CC ?= cc
debug: export BOA_CC ?= cc
debug: build

.PHONY: valgrind
valgrind: export OPTIM_FLAGS ?= -O0 -ggdb3 -Werror
valgrind: export CC ?= cc
valgrind: export BOA_CC ?= cc
valgrind: run-valgrind

.PHONY: sanitize
sanitize: export OPTIM_FLAGS ?= -Os -ggdb3 -Werror -fsanitize=address
sanitize: export CC ?= cc
sanitize: export BOA_CC ?= cc
sanitize: build

COMPILER_SRC=$(wildcard compiler/*.c)
COMPILER_OBJ=$(patsubst compiler/%.c,obj/compiler/%.o,$(COMPILER_SRC))
COMPILER_INC=$(wildcard compiler/*.h) $(wildcard util/*.h)

UTIL_SRC=$(wildcard util/*.c)
UTIL_OBJ=$(patsubst util/%.c,obj/util/%.o,$(UTIL_SRC))
UTIL_INC=$(wildcard util/*.h) $(wildcard util/*.h)
COMPILER_DEPS = $(COMPILER_OBJ) $(UTIL_OBJ) y.tab.o lex.yy.o

y.tab.o: grammar/grammar.y util/*.h
	yacc -y -d grammar/grammar.y
	$(CC) -std=$(STD) $(OPTIM_FLAGS) $(CFLAGS) $(INCLD) -c y.tab.c -o y.tab.o

lex.yy.o: y.tab.o grammar/tokens.l
	lex grammar/tokens.l
	$(CC) -std=$(STD) $(OPTIM_FLAGS) $(CFLAGS) $(INCLD) -c lex.yy.c -o lex.yy.o

boa: $(COMPILER_DEPS)
	$(CC) -std=$(STD) $(WARN_FLAGS) -o boa $(COMPILER_DEPS) $(OPTIM_FLAGS) $(CFLAGS)

obj/compiler/%.o: compiler/%.c $(COMPILER_INC)
	$(CC) -o $@ -c $< -std=$(STD) $(CFLAGS) $(OPTIM_FLAGS) $(WARN_FLAGS)

obj/util/%.o: util/%.c $(UTIL_INC)
	$(CC) -o $@ -c $< -std=$(STD) $(CFLAGS) $(OPTIM_FLAGS) $(WARN_FLAGS)

runtime/runtime.o: runtime/runtime.c runtime/runtime.h
	$(BOA_CC) -std=$(STD) $(WARN_FLAGS) $(OPTIM_FLAGS) $(INCLD) $(CFLAGS) \
		-c runtime/runtime.c -o runtime/runtime.o

runtime/boaobj.o: runtime/boaobj.c runtime/boaobj.h
	$(BOA_CC) -std=$(STD) $(WARN_FLAGS) $(OPTIM_FLAGS) $(INCLD) $(CFLAGS) \
		-c runtime/boaobj.c -o runtime/boaobj.o

libboaruntime.a: runtime/runtime.o runtime/boaobj.o
	ar rcs libboaruntime.a runtime/*.o

.PHONY: build
build: boa libboaruntime.a

.PHONY: run-valgrind
run-valgrind: build
	valgrind ./boa -b -o bin/ex16 example/ex16.boa

.PHONY: clean
clean:
	rm -rf bin/*
	rm -f boa
	rm -f libboaruntime.a runtime.o
	rm -f y.tab.h y.tab.c lex.yy.c lex.yy.o y.tab.o
	rm -f core
	rm -f vgcore*
	rm -f example/*.c
	rm -f *.o
	rm -f example/ex[0-9]+
	rm -f ex[0-9]+
	rm -f ex16
	rm -f a.out
	rm -f hello
	rm -f example/hello
	rm -f runtime/*.o
	rm -f obj/*.o obj/compiler/*.o obj/util/*.o
