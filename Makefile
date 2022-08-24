YACC=bison
LEX=flex
STD ?= c89
WARN_FLAGS ?= -Wall -Wextra -Wpedantic -Wno-padded
INCLD ?= -I.
CFLAGS ?= -fPIC -pipe

.PHONY: release
release: dynamic

.PHONY: static
static: export OPTIM_FLAGS ?= -Os -static -s
static: export CC := musl-gcc
static: export BOACC := musl-gcc
static: build

.PHONY: clang-warn-everything
clang-warn-everything: export WARN_FLAGS := \
	-Weverything \
	-Wno-format-nonliteral \
	-Wno-padded \
	-Wno-float-equal
clang-warn-everything: export CC := clang
clang-warn-everything: export BOACC := clang
clang-warn-everything: build

.PHONY: dynamic
dynamic: export OPTIM_FLAGS ?= -Os -flto -fstack-protector-all -D_FORTIFY_SOURCE=2
dynamic: export CC ?= cc
dynamic: export BOACC ?= cc
dynamic: build

.PHONY: debug
debug: export OPTIM_FLAGS ?= -O0 -ggdb3 -Werror -DDEBUG
debug: export CC ?= cc
debug: export BOACC ?= cc
debug: build

.PHONY: valgrind
valgrind: export OPTIM_FLAGS ?= -O0 -ggdb3 -Werror
valgrind: export CC ?= cc
valgrind: export BOACC ?= cc
valgrind: run-valgrind

.PHONY: sanitize
sanitize: export OPTIM_FLAGS ?= -Os -ggdb3 -Werror -fsanitize=address
sanitize: export CC ?= cc
sanitize: export BOACC ?= cc
sanitize: build

COMPILER_SRC=$(wildcard compiler/*.c)
COMPILER_OBJ=$(patsubst compiler/%.c,obj/compiler/%.o,$(COMPILER_SRC))
COMPILER_INC=$(wildcard compiler/*.h) $(wildcard util/*.h)

UTIL_SRC=$(wildcard util/*.c)
UTIL_OBJ=$(patsubst util/%.c,obj/util/%.o,$(UTIL_SRC))
UTIL_INC=$(wildcard util/*.h) $(wildcard util/*.h)
COMPILER_DEPS = $(COMPILER_OBJ) $(UTIL_OBJ) obj/grammar/y.tab.o obj/grammar/lex.yy.o

RUNTIME_SRC=$(wildcard runtime/*.c)
RUNTIME_OBJ=$(patsubst runtime/%.c,obj/runtime/%.o,$(RUNTIME_SRC))
RUNTIME_INC=$(wildcard runtime/*.h) $(wildcard runtime/*.h)
RUNTIME_DEPS = $(RUNTIME_OBJ) $(UTIL_OBJ)

obj/grammar/y.tab.o: grammar/grammar.y util/*.h compiler/*.c compiler/*.h
	$(YACC) -o obj/grammar/y.tab.c -y -d grammar/grammar.y -Wno-yacc
	$(CC) -std=$(STD) $(OPTIM_FLAGS) $(CFLAGS) $(INCLD) \
		-c obj/grammar/y.tab.c \
		-o obj/grammar/y.tab.o

obj/grammar/lex.yy.o: obj/grammar/y.tab.o grammar/tokens.l compiler/*.c compiler/*.h
	$(LEX) -o obj/grammar/lex.yy.c grammar/tokens.l
	$(CC) -std=$(STD) $(OPTIM_FLAGS) $(CFLAGS) $(INCLD) \
		-c obj/grammar/lex.yy.c \
		-o obj/grammar/lex.yy.o -I obj/grammar

bin/boa: $(COMPILER_DEPS)
	$(CC) -std=$(STD) $(WARN_FLAGS) -o bin/boa $(COMPILER_DEPS) $(OPTIM_FLAGS) $(CFLAGS)

lib/libboaruntime.a: $(RUNTIME_DEPS)
	ar rcs lib/libboaruntime.a $(RUNTIME_DEPS)

obj/compiler/%.o: compiler/%.c $(COMPILER_INC)
	$(CC) -o $@ -c $< -std=$(STD) $(CFLAGS) $(OPTIM_FLAGS) $(WARN_FLAGS)

obj/util/%.o: util/%.c $(UTIL_INC)
	$(CC) -o $@ -c $< -std=$(STD) $(CFLAGS) $(OPTIM_FLAGS) $(WARN_FLAGS)

obj/runtime/%.o: runtime/%.c $(RUNTIME_INC)
	$(BOACC) -o $@ -c $< -std=$(STD) $(CFLAGS) $(OPTIM_FLAGS) $(WARN_FLAGS)

.PHONY: build
build: bin/boa lib/libboaruntime.a

.PHONY: run-valgrind
run-valgrind: build
	valgrind ./boa -b -o bin/ex16 example/ex16.boa

.PHONY: clean
clean:
	rm -f bin/*
	rm -f lib/*
	rm -f boa
	rm -f libboaruntime.a runtime.o
	rm -f runtime/*.o
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
	rm -f obj/*.o obj/compiler/*.o obj/util/*.o obj/grammar/*
	rm -f obj/runtime/*.o
