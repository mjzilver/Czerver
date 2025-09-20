DEBUG ?= 0

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TEST_DIR := test

SRC_FILES := $(shell find $(SRC_DIR) -name "*.c")
HEAD_FILES := $(shell find $(SRC_DIR) -name "*.h")

OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
OBJ_DIRS := $(sort $(dir $(OBJ_FILES)))
LIB_OBJS := $(filter-out $(OBJ_DIR)/main.o,$(OBJ_FILES))

TEST_FILES := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ_FILES := $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%.test.o,$(TEST_FILES))

CC := gcc
CFLAGS := -D_POSIX_C_SOURCE=200809L -Wall -g -std=c11 -I$(SRC_DIR)
PKG_CONFIG := pkg-config
LUA_PKG := lua
CHECK_PKG := check

LUA_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(LUA_PKG))
LUA_LIBS   := $(shell $(PKG_CONFIG) --libs $(LUA_PKG))

CHECK_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(CHECK_PKG))
CHECK_LIBS   := $(shell $(PKG_CONFIG) --libs $(CHECK_PKG))

SAN_FLAGS := -fsanitize=address,undefined -fno-omit-frame-pointer

ifeq ($(DEBUG),1)
    CFLAGS += $(SAN_FLAGS)
    LDFLAGS += $(SAN_FLAGS)
endif

.PHONY: all build run valgrind leaks clean format tidy test port-kill

# ---------------- Main Program ----------------
all: build run

run: build
	@$(BIN_DIR)/main

build: $(BIN_DIR)/main

$(BIN_DIR)/main: $(OBJ_FILES) | $(BIN_DIR)
	$(CC) -o $@ $(OBJ_FILES) $(LUA_LIBS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LUA_CFLAGS) -c $< -o $@

$(OBJ_DIRS):
	@mkdir -p $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# ---------------- Test Build ----------------
test: $(BIN_DIR)/run_tests
	@$(BIN_DIR)/run_tests

$(BIN_DIR)/run_tests: $(LIB_OBJS) $(TEST_OBJ_FILES) | $(BIN_DIR)
	$(CC) -o $@ $(LIB_OBJS) $(TEST_OBJ_FILES) $(CHECK_LIBS) $(LUA_LIBS) $(LDFLAGS)

$(OBJ_DIR)/%.test.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) -c $< -o $@

# ---------------- Tools ----------------
valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BIN_DIR)/main

leaks: build
	./scripts/leaks.sh

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

format:
	clang-format -i $(SRC_FILES) $(HEAD_FILES)

tidy:
	clang-tidy $(SRC_FILES) $(HEAD_FILES) -- $(CFLAGS) $(LUA_CFLAGS) $(CHECK_CFLAGS)