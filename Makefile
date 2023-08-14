#
# Compiler setup
#
CC ?= gcc
VERSION = 0.0.1

#
# Compiler Flags
#
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer -Wno-format-security
CFLAGS := -Werror -Wall --std=c18 -DEXEC_VERSION=$(VERSION)
LDFLAGS += -lpthread
DEBUG_FLAGS = $(ASAN_FLAGS) -O0 -g3

#
# Project Files
#
BUILDDIR = build
SOURCEDIR = src
SOURCES := $(shell find $(SOURCEDIR) -name '*.c')
OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))
BIN = iproc

#
# Helper Vars
#
RM = rm -rf
MKDIR = mkdir

.PHONY: all clean setup

all: $(BIN)


$(BIN): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(BIN)

$(BUILDDIR)/%.o: %.c
	@mkdir -p "$(shell dirname "$@")"
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(HEADERDIR) -I$(dir $<) -c $< -o $@


debug: CFLAGS += $(DEBUG_FLAGS) -DDEBUG
debug: $(BIN)

setup:
	$(MKDIR) -p $(BUILDDIR)

clean:
	$(RM) $(BIN) $(OBJECTS)

distclean: clean
