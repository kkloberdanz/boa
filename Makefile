# Credit for this Makefile: https://github.com/mbcrawfo/GenericMakefile
#
#
#### PROJECT SETTINGS ####
# The name of the executable to be created
BIN_NAME := boa
# Compiler used
CC = gcc
# Extension of source files used in the project
SRC_EXT = c
# Path to the source directory, relative to the makefile
SRC_PATH = .
# Space-separated pkg-config libraries used by this project
LIBS =
SRC_ONLY_FLAGS =
# General compiler flags

STATIC = -static

COMPILE_FLAGS = \
	-std=iso9899:1990 \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wswitch \
	-ggdb \
	$(STATIC)

# Additional release-specific flags
RCOMPILE_FLAGS = -D NDEBUG
# Additional debug-specific flags
DCOMPILE_FLAGS = -D DEBUG
# Add additional include paths
INCLUDES = -I $(SRC_PATH)
# General linker settings
LINK_FLAGS = -lfl -flto $(STATIC)
# Additional release-specific linker settings
RLINK_FLAGS =
# Additional debug-specific linker settings
DLINK_FLAGS =
# Destination directory, like a jail or mounted system
DESTDIR = /
# Install path (bin/ is appended automatically)
INSTALL_PREFIX = usr/local
#### END PROJECT SETTINGS ####

# Optionally you may move the section above to a separate config.mk file, and
# uncomment the line below
# include config.mk

# Generally should not need to edit below this line

# Obtains the OS type, either 'Darwin' (OS X) or 'Linux'
UNAME_S:=$(shell uname -s)

# Function used to check variables. Use on the command line:
# make print-VARNAME
# Useful for debugging and adding features
print-%: ; @echo $*=$($*)

# Shell used in this makefile
# bash is used for 'echo -en'
SHELL = /bin/bash
# Clear built-in rules
.SUFFIXES:
# Programs for installation
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

# Append pkg-config specific libraries if need be
ifneq ($(LIBS),)
	COMPILE_FLAGS += $(shell pkg-config --cflags $(LIBS))
	LINK_FLAGS += $(shell pkg-config --libs $(LIBS))
endif

# Verbose option, to output compile and link commands
export V := false
export CMD_PREFIX := @
ifeq ($(V),true)
	CMD_PREFIX :=
endif

# Combine compiler and linker flags
release: export CFLAGS := $(CFLAGS) \
	$(COMPILE_FLAGS) \
	$(RCOMPILE_FLAGS)

release: export LDFLAGS := $(LDFLAGS) \
	$(LINK_FLAGS) \
	$(RLINK_FLAGS)

release: export BUILD_TYPE := release

release: export OPTIM_FLAGS := -Os

debug: export OPTIM_FLAGS := -O0

debug: export CFLAGS := \
	$(CFLAGS) \
	$(COMPILE_FLAGS) \
	$(DCOMPILE_FLAGS) \
	-Werror
debug: export LDFLAGS := $(LDFLAGS) $(LINK_FLAGS) $(DLINK_FLAGS) -O0 -Werror
debug: export BUILD_TYPE := debug

# Build and output paths
release: export BUILD_PATH := build/release
release: export BIN_PATH := bin/release
debug: export BUILD_PATH := build/debug
debug: export BIN_PATH := bin/debug
install: export BIN_PATH := bin/release

# Find all source files in the source directory, sorted by most
# recently modified
ifeq ($(UNAME_S),Darwin)
	SOURCES = $(shell find $(SRC_PATH) -name '*.$(SRC_EXT)' \
	   -not -path "./example/*" \
	   -not -path "./runtime/*" | sort -k 1nr | cut -f2-)
else
	SOURCES = $(shell find $(SRC_PATH) -name '*.$(SRC_EXT)' \
	   -not -path "./example/*" -not -path "./runtime/*" \
	   -printf '%T@\t%p\n' | sort -k 1nr | cut -f2-)
endif

# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)
# Set the dependency files that will be used to add header dependencies
DEPS = $(OBJECTS:.o=.d)

# Macros for timing compilation
ifeq ($(UNAME_S),Darwin)
	CUR_TIME = awk 'BEGIN{srand(); print srand()}'
	TIME_FILE = $(dir $@).$(notdir $@)_time
	START_TIME = $(CUR_TIME) > $(TIME_FILE)
	END_TIME = read st < $(TIME_FILE) ; \
		$(RM) $(TIME_FILE) ; \
		st=$$((`$(CUR_TIME)` - $$st)) ; \
		echo $$st
else
	TIME_FILE = $(dir $@).$(notdir $@)_time
	START_TIME = date '+%s' > $(TIME_FILE)
	END_TIME = read st < $(TIME_FILE) ; \
		$(RM) $(TIME_FILE) ; \
		st=$$((`date '+%s'` - $$st - 86400)) ; \
		echo `date -u -d @$$st '+%H:%M:%S'`
endif

# Version macros
# Comment/remove this section to remove versioning
USE_VERSION := false
# If this isn't a git repo or the repo has no tags,
# git describe will return non-zero
SED_EXPR = 's/v\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)\
		   -\?.*-\([0-9]*\)-\(.*\)/\1 \2 \3 \4 \5/g'
ifeq ($(shell git describe > /dev/null 2>&1 ; echo $$?), 0)
	USE_VERSION := true
	VERSION := $(shell git describe --tags --long --dirty --always | \
		sed $(SED_EXPR))
	VERSION_MAJOR := $(word 1, $(VERSION))
	VERSION_MINOR := $(word 2, $(VERSION))
	VERSION_PATCH := $(word 3, $(VERSION))
	VERSION_REVISION := $(word 4, $(VERSION))
	VERSION_HASH := $(word 5, $(VERSION))
	VERSION_STRING := \
		"$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)\
		.$(VERSION_REVISION)-$(VERSION_HASH)"
	override CFLAGS := $(CFLAGS) \
		-D VERSION_MAJOR=$(VERSION_MAJOR) \
		-D VERSION_MINOR=$(VERSION_MINOR) \
		-D VERSION_PATCH=$(VERSION_PATCH) \
		-D VERSION_REVISION=$(VERSION_REVISION) \
		-D VERSION_HASH=\"$(VERSION_HASH)\"
endif

# Standard, non-optimized release build
.PHONY: release
release: dirs
ifeq ($(USE_VERSION), true)
	@echo "Beginning release build v$(VERSION_STRING)"
else
	@echo "Beginning release build"
endif
	@$(START_TIME)
	@$(MAKE) all --no-print-directory
	@echo -n "Total build time: "
	@$(END_TIME)

# Debug build for gdb debugging
.PHONY: debug
debug: dirs
ifeq ($(USE_VERSION), true)
	@echo "Beginning debug build v$(VERSION_STRING)"
else
	@echo "Beginning debug build"
endif
	@$(START_TIME)
	@$(MAKE) all --no-print-directory
	@echo -n "Total build time: "
	@$(END_TIME)

# Create the directories used in the build
.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN_PATH)

# Installs to the set path
.PHONY: install
install:
	@echo "Installing to $(DESTDIR)$(INSTALL_PREFIX)/bin"
	@$(INSTALL_PROGRAM) $(BIN_PATH)/$(BIN_NAME) $(DESTDIR)$(INSTALL_PREFIX)/bin

# Uninstalls the program
.PHONY: uninstall
uninstall:
	@echo "Removing $(DESTDIR)$(INSTALL_PREFIX)/bin/$(BIN_NAME)"
	@$(RM) $(DESTDIR)$(INSTALL_PREFIX)/bin/$(BIN_NAME)

# Removes all build files
.PHONY: clean
clean:
	@echo "Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting directories"
	@$(RM) -r build
	@$(RM) -r bin
	@$(RM) -f y.tab.c y.tab.h lex.yy.c
	@$(RM) -f example/*.c
	@$(RM) -f a.out
	@$(RM) -f core
	@$(RM) -f libruntime.a runtime.o

.PHONY: runtime
runtime:
	$(CC) $(OPTIM_FLAGS) $(CFLAGS) $(SRC_ONLY_FLAGS) \
		-c runtime/runtime.c \
		-o build/$(BUILD_TYPE)/runtime.o
	ar rcs libruntime.a build/$(BUILD_TYPE)/runtime.o


.PHONY: lexer
lexer: parser
	lex grammar/tokens.l
	$(CC) $(OPTIM_FLAGS) $(CFLAGS) -c lex.yy.c -o build/$(BUILD_TYPE)/lex.yy.o

.PHONY: parser
parser:
	yacc -y -d grammar/grammar.y
	$(CC) $(OPTIM_FLAGS) $(CFLAGS) -c y.tab.c -o build/$(BUILD_TYPE)/y.tab.o

# Main rule, checks the executable and symlinks to the output
all: $(BIN_PATH)/$(BIN_NAME)
	@echo "Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	@ln -s $(BIN_PATH)/$(BIN_NAME) $(BIN_NAME)

# Link the executable
$(BIN_PATH)/$(BIN_NAME): $(OBJECTS) lexer parser runtime
	@echo "Linking: $@"
	@$(START_TIME)
	$(CMD_PREFIX)$(CC) $(SRC_ONLY_FLAGS) $(OBJECTS) $(LDFLAGS) -o $@
	@echo -en "\t Link time: "
	@$(END_TIME)

# Add dependency files, if they exist
-include $(DEPS)

# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@"
	@$(START_TIME)
	$(CMD_PREFIX)$(CC) $(SRC_ONLY_FLAGS) $(OPTIM_FLAGS) \
		$(CFLAGS) $(INCLUDES) -c $< -o $@
	@echo -en "\t Compile time: "
	@$(END_TIME)
