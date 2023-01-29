# TODO: Now it doesn't work.

#BUILD = RELEASE
BUILD = DEBUG

PROGRAM := differentiator
RUN_ARGS :=

EXT_C := c
EXT_H := h
EXT_CXX := cpp
EXT_HXX := hpp
EXT_OBJ := o
EXT_DEPEND := d

CC := gcc
CXX := g++
LD := gcc
DEBUGGER := gdb

CSTD := c11
CXXSTD := c++17

LDLIBS := -lm

FLAGS_WARN := -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith \
    -Wcast-align -Wstrict-prototypes -Wstrict-overflow=2 -Wwrite-strings \
    -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion \
    -Wunreachable-code -Wformat=2 -Winit-self -Wuninitialized \
    -Wpointer-arith -Wold-style-definition -Wstrict-prototypes \
    -Wmissing-prototypes -Werror-implicit-function-declaration -Wlogical-op \
    -Wduplicated-cond -Wcast-qual -Wcast-align -Wformat-security \
    -Wlarger-than=32768 -Wmissing-declarations \
    -Wchar-subscripts -Wno-varargs -Wunused -Wvariadic-macros \
    -Wno-missing-field-initializers -Wno-narrowing -Wswitch-default \
    -Wsync-nand -Wsuggest-final-types -Wsign-conversion \
    -Wsuggest-attribute=noreturn -Winline -Wopenmp-simd -Wpacked \
    -Wredundant-decls -Waggressive-loop-optimizations -Wempty-body \
    -Wformat-nonliteral -Wformat-signedness

FLAGS_CHECK := -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr \
    -pie -fcheck-new -fstack-check -fstack-protector -fstrict-overflow \
    -flto-odr-type-merging -fno-omit-frame-pointer -fPIE

CFLAGS := -std=$(CSTD) $(FLAGS_WARN)
CXXFLAGS := -std=$(CXXSTD) $(FLAGS_WARN)


CFLAGS_DEBUG := -Werror -ggdb3 -O0 $(FLAGS_CHECK)
CFLAGS_RELEASE := -O2 -D NDEBUG
CXXFLAGS_DEBUG := $(CFLAGS_DEBUG)
CXXFLAGS_RELEASE := $(CFLAGS_RELEASE)
DEPFLAGS := -M

SRC_DIR := src
RELEASE_DIR := release
DEBUG_DIR := debug

.PHONY: all build clean debug run

ifeq ($(BUILD), DEBUG)
  :w
    BUILD_DIR := build/$(DEBUG_DIR)
    TARGET_DIR := $(DEBUG_DIR)
else
    BUILD_DIR := build
    TARGET_DIR := $(RELEASE_DIR)
endif


TARGET := $(TARGET_DIR)/$(PROGRAM)

CSRC += $(wildcard $(SRC_DIR)/**/*.$(EXT_C)))
HSRC := $(wildcard $(SRC_DIR)/**/*.$(EXT_H))
CXXSRC := $(wildcard $(SRC_DIR)/**/*.$(EXT_CXX))
HXXSRC := $(wildcard $(SRC_DIR)/**/*.$(EXT_HXX))
COBJ := $(patsubst %.$(EXT_C),$(BUILD_DIR)/%.$(EXT_OBJ),$(notdir $(CSRC)))
CXXOBJ := $(patsubst %.$(EXT_CXX),$(BUILD_DIR)/%.$(EXT_OBJ),$(notdir $(CXXSRC)))
DEPEND := $(patsubst %.$(EXT_OBJ),%.$(EXT_C).$(EXT_DEPEND),$(COBJ))
DEPEND += $(patsubst %.$(EXT_OBJ),%.$(EXT_CXX).$(EXT_DEPEND),$(CXXOBJ))

vpath %.(EXT_DEPEND) $(BUILD_DIR)
vpath %.(EXT_OBJ) $(BUILD_DIR)
vpath %.(EXT_C) $(SRC_DIR)
vpath %.(EXT_CXX) $(SRC_DIR)
$(warning vpath=$(VPATH))

all: build

release: CFLAGS += $(CFLAGS_RELEASE)
release: CXXFLAGS += $(CXXFLAGS_RELEASE)
release: all

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: CXXFLAGS += $(CXXFLAGS_DEBUG)
debug: all

build: $(TARGET)

run: build
	./$(TARGET) $(RUN_ARGS)

gdb: build
	$(DEBUGGER) ./$(TARGET)

clean:
	rm -rf $(RELEASE_DIR) $(BUILD_DIR) $(DEBUG_DIR) && true

$(TARGET): $(DEPEND) $(COBJ) $(CXXOBJ)
	mkdir -p $(TARGET_DIR)
	$(LD) $(LDFLAGS) -o $@ $(BUILD_DIR)/*.o

%.(EXT_DEPEND): %.(EXT_C)
	$(CC) $(CFLAGS) $(DEPFLAGS) $< >$@

%.(EXT_DEPEND): %.(EXT_CXX)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $< >$@

%.(EXT_OBJ): %.(EXT_C)
	$(CC) $(CFLAGS) -c $< -o $@

%.(EXT_OBJ): %.(EXT_CXX)
	$(CXX) $(CXXFLAGS) -c $< -o $@

sinclude $(wildcard $(BUILD_DIR)/*.$(EXT_DEPEND))
