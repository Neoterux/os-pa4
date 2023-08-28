#
# Compiler setup
#
CC ?= gcc
VERSION ="0.0.5a"

#
# Compiler Flags
#
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer -Wno-format-security
CFLAGS := -Werror -Wall --std=c18 -DEXEC_VERSION=$(VERSION)
LDFLAGS += -lpthread
DEBUG_FLAGS = -O0 -g3

#
# Project Files
#
BUILDDIR = build
SOURCEDIR = src
TESTDIR = tests
SOURCES := $(shell find $(SOURCEDIR) -name '*.c')
OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))
BIN = iproc

NMSOURCES = $(shell find $(SOURCEDIR) \( -iname '*.c' ! -iname 'main.c' \))
NMOBJECTS := $(addprefix $(BUILDDIR)/,$(NMSOURCES:%.c=%.o))
TEST_SOURCES := $(shell find $(TESTDIR) -name '*.c')
TEST_OBJECTS := $(addprefix $(BUILDDIR)/,$(TEST_SOURCES:%.c=%.o))

#
# Helper Vars
#
RM = rm -rf
MKDIR = mkdir

.PHONY: all clean setup

all: $(BIN)


$(BIN): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(BUILDDIR)/$(BIN)

$(BUILDDIR)/%.o: %.c
	@mkdir -p "$(shell dirname "$@")"
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(HEADERDIR) -I$(dir $<) -c $< -o $@

memleak: CFLAGS += $(ASAN_FLAGS)
memleak: $(BIN)

debug: CFLAGS += $(DEBUG_FLAGS) -DDEBUG
debug: $(BIN)

setup:
	$(MKDIR) -p $(BUILDDIR)

clean:
	$(RM) $(BIN) $(OBJECTS)

build_test: CFLAGS += -fPIC
build_test: $(NMOBJECTS) $(TEST_OBJECTS)
	$(CC) -lcunit -I$(SOURCEDIR) $(NMOBJECTS) $(TEST_OBJECTS) -o $(BUILDDIR)/test-bin

test: build_test


distclean: clean
