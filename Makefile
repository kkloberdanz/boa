CC=clang -static

STD=c89

WARN_FLAGS=-Wall -Wextra -Wpedantic -Weverything -Wno-padded

DEBUG=

.PHONY: release
release: export OPTIM_FLAGS := -Os
release: build

.PHONY: debug
debug: export OPTIM_FLAGS := -O0 -ggdb -Werror
debug: build

.PHONY: parser
parser:
	yacc -y -d grammar/grammar.y
	$(CC) -std=$(STD) $(OPTIM_FLAGS) $(CFLAGS) $(DEBUG) -c y.tab.c -o y.tab.o

.PHONY: lexer
lexer: parser
	lex grammar/tokens.l
	$(CC) -std=$(STD) $(OPTIM_FLAGS) $(CFLAGS) $(DEBUG) -c lex.yy.c -o lex.yy.o

.PHONY: binary
binary: parser lexer
	$(CC) -std=$(STD) $(WARN_FLAGS) -o iba \
		compiler/*.c util/*.c lex.yy.o y.tab.o \
		$(OPTIM_FLAGS) $(CFLAGS) $(DEBUG) -lfl

.PHONY: library
library:
	$(CC) -std=$(STD) $(WARN_FLAGS) $(OPTIM_FLAGS) $(DEBUG) $(CFLAGS) \
		-c runtime/runtime.c -o runtime.o
	ar rcs libruntime.a runtime.o

.PHONY: build
build: binary library

.PHONY: clean
clean:
	rm -rf bin
	rm -f iba
	rm -f libruntime.a runtime.o
	rm -f y.tab.h y.tab.c lex.yy.c lex.yy.o y.tab.o
	rm -f core
	rm -f example/*.c
