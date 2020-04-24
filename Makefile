CC ?= cc

STD ?= c89

WARN_FLAGS ?= -Wall -Wextra -Wpedantic -Wno-padded

DEBUG ?=

.PHONY: release
release: export OPTIM_FLAGS := -Os
release: build

.PHONY: static
static: export OPTIM_FLAGS := -Os -static
static: build

.PHONY: debug
debug: export OPTIM_FLAGS := -O0 -ggdb -Werror
debug: build

y.tab.o: grammar/grammar.y
	yacc -y -d grammar/grammar.y
	$(CC) -std=$(STD) $(OPTIM_FLAGS) $(CFLAGS) $(DEBUG) -c y.tab.c -o y.tab.o

lex.yy.o: y.tab.o grammar/tokens.l
	lex grammar/tokens.l
	$(CC) -std=$(STD) $(OPTIM_FLAGS) $(CFLAGS) $(DEBUG) -c lex.yy.c -o lex.yy.o

iba: y.tab.o lex.yy.o compiler/*.c util/*.c  compiler/*.h util/*.h
	$(CC) -std=$(STD) $(WARN_FLAGS) -o iba \
		compiler/*.c util/*.c lex.yy.o y.tab.o \
		$(OPTIM_FLAGS) $(CFLAGS) $(DEBUG) -lfl

libruntime.a:
	$(CC) -std=$(STD) $(WARN_FLAGS) $(OPTIM_FLAGS) $(DEBUG) $(CFLAGS) \
		-c runtime/runtime.c -o runtime.o
	ar rcs libruntime.a runtime.o

.PHONY: build
build: iba libruntime.a

.PHONY: clean
clean:
	rm -rf bin
	rm -f iba
	rm -f libruntime.a runtime.o
	rm -f y.tab.h y.tab.c lex.yy.c lex.yy.o y.tab.o
	rm -f core
	rm -f example/*.c
