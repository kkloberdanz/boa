STD ?= c89

WARN_FLAGS ?= -Wall -Wextra -Wpedantic -Wno-padded

INCLD ?= -I.

CFLAGS ?= -fPIC

.PHONY: release
release: dynamic

.PHONY: static
static: export OPTIM_FLAGS ?= -Os -static
static: export CC := musl-gcc
static: export BOA_CC := musl-gcc
static: build

.PHONY: clang-warn-everything
clang-warn-everything: export WARN_FLAGS := \
	-Weverything \
	-Wno-format-nonliteral \
	-Wno-padded
clang-warn-everything: export CC := clang
clang-warn-everything: export BOA_CC := clang
clang-warn-everything: build

.PHONY: dynamic
dynamic: export OPTIM_FLAGS ?= -Os
dynamic: export CC ?= cc
dynamic: export BOA_CC ?= cc
dynamic: build

.PHONY: debug
debug: export OPTIM_FLAGS ?= -O0 -ggdb3 -Werror
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

COMPILER_DEPS = compiler/*.c util/*.c  compiler/*.h util/*.h
COMPILER_SRC = compiler/*.c util/*.c

y.tab.o: grammar/grammar.y
	yacc -y -d grammar/grammar.y
	$(CC) -std=$(STD) $(OPTIM_FLAGS) $(CFLAGS) $(INCLD) -c y.tab.c -o y.tab.o

lex.yy.o: y.tab.o grammar/tokens.l
	lex grammar/tokens.l
	$(CC) -std=$(STD) $(OPTIM_FLAGS) $(CFLAGS) $(INCLD) -c lex.yy.c -o lex.yy.o

boa: y.tab.o lex.yy.o $(COMPILER_DEPS)
	$(CC) -std=$(STD) $(WARN_FLAGS) -o boa \
		$(COMPILER_SRC) lex.yy.o y.tab.o \
		$(OPTIM_FLAGS) $(CFLAGS) $(INCLD)

bin/tgc.o:
	$(BOA_CC) -o bin/tgc.o -c extern/tgc/tgc.c -Os -fPIC

libruntime.a: runtime/*.c runtime/*.h bin/tgc.o
	$(BOA_CC) -std=$(STD) $(WARN_FLAGS) $(OPTIM_FLAGS) $(INCLD) $(CFLAGS) \
		-c runtime/runtime.c -o runtime.o
	ar rcs libruntime.a runtime.o bin/tgc.o

libccruntime.a: runtime/*.c runtime/*.h bin/tgc.o
	cc -std=$(STD) -Os $(INCLD) -fPIC \
		-c runtime/runtime.c -o ccruntime.o
	ar rcs libccruntime.a ccruntime.o bin/tgc.o

.PHONY: build
build: boa libruntime.a libccruntime.a

.PHONY: run-valgrind
run-valgrind: build
	valgrind ./boa -b -o bin/ex16 example/ex16.boa

.PHONY: clean
clean:
	rm -rf bin/*
	rm -f boa
	rm -f libruntime.a runtime.o libccruntime.a
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
