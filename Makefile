CC ?= cc
IBA_CC ?= cc

STD ?= c89

WARN_FLAGS ?= -Wall -Wextra -Wpedantic -Wno-padded

INCLD ?= -I.

CFLAGS ?= -fPIC

.PHONY: release
release: export OPTIM_FLAGS ?= -Os -static
release: export CC ?= musl-gcc
release: export IBA_CC ?= musl-gcc
release: build

.PHONY: dynamic
dynamic: export OPTIM_FLAGS ?= -Os
dynamic: build

.PHONY: debug
debug: export OPTIM_FLAGS ?= -O0 -ggdb -Werror
debug: build

y.tab.o: grammar/grammar.y
	yacc -y -d grammar/grammar.y
	$(CC) -std=$(STD) $(OPTIM_FLAGS) $(CFLAGS) $(INCLD) -c y.tab.c -o y.tab.o

lex.yy.o: y.tab.o grammar/tokens.l
	lex grammar/tokens.l
	$(CC) -std=$(STD) $(OPTIM_FLAGS) $(CFLAGS) $(INCLD) -c lex.yy.c -o lex.yy.o

iba: y.tab.o lex.yy.o compiler/*.c util/*.c  compiler/*.h util/*.h
	$(CC) -std=$(STD) $(WARN_FLAGS) -o iba \
		compiler/*.c util/*.c lex.yy.o y.tab.o \
		$(OPTIM_FLAGS) $(CFLAGS) $(INCLD)

libruntime.a: runtime/*.c runtime/*.h
	$(IBA_CC) -std=$(STD) $(WARN_FLAGS) $(OPTIM_FLAGS) $(INCLD) $(CFLAGS) \
		-c runtime/runtime.c -o runtime.o
	ar rcs libruntime.a runtime.o

libccruntime.a: runtime/*.c runtime/*.h
	cc -std=$(STD) $(WARN_FLAGS) $(OPTIM_FLAGS) $(INCLD) -fPIC \
		-c runtime/runtime.c -o ccruntime.o
	ar rcs libccruntime.a ccruntime.o

.PHONY: build
build: iba libruntime.a libccruntime.a

.PHONY: clean
clean:
	rm -rf bin
	rm -f iba
	rm -f libruntime.a runtime.o libccruntime.a
	rm -f y.tab.h y.tab.c lex.yy.c lex.yy.o y.tab.o
	rm -f core
	rm -f example/*.c
	rm -f *.o
