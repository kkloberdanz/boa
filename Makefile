CC=clang -static

WARN_FLAGS=-Wall -Wextra -Wpedantic -Weverything

.PHONY: release
release: export OPTIM_FLAGS := -Os
release: build

.PHONY: debug
debug: export OPTIM_FLAGS := -O0 -g
debug: build

.PHONY: parser
parser:
	yacc -y -d grammar/grammar.y
	$(CC) $(OPTIM_FLAGS) $(CFLAGS) -c y.tab.c -o y.tab.o

.PHONY: lexer
lexer: parser
	lex grammar/tokens.l
	$(CC) $(OPTIM_FLAGS) $(CFLAGS) -c lex.yy.c -o lex.yy.o

.PHONY: binary
binary: parser lexer
	$(CC) $(WARN_FLAGS) -o boa compiler/*.c util/*.c lex.yy.o y.tab.o \
		$(OPTIM_FLAGS) $(CFLAGS) -lfl

.PHONY: library
library:
	$(CC) $(WARN_FLAGS) $(OPTIM_FLAGS) $(CFLAGS) \
		-c runtime/runtime.c -o runtime.o
	ar rcs libruntime.a runtime.o

.PHONY: build
build: binary library

.PHONY: clean
clean:
	rm -rf bin
	rm -f boa
	rm -f libruntime.a runtime.o
	rm -f y.tab.h y.tab.c lex.yy.c lex.yy.o y.tab.o
	rm -f core
	rm -f example/*.c
