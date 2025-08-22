SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = test

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
HEAD_FILES = $(wildcard $(SRC_DIR)/*.h)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
LIB_OBJS := $(filter-out $(OBJ_DIR)/main.o,$(OBJ_FILES))

TEST_FILES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ_FILES = $(TEST_FILES:$(TEST_DIR)/%.c=$(OBJ_DIR)/%.test.o)

CC = gcc
CFLAGS = -Wall -g -std=c11 -I$(SRC_DIR)
PKG_CONFIG = pkg-config
LUA_PKG = lua
CHECK_PKG = check

LUA_CFLAGS = $(shell $(PKG_CONFIG) --cflags $(LUA_PKG))
LUA_LIBS   = $(shell $(PKG_CONFIG) --libs $(LUA_PKG))

CHECK_CFLAGS = $(shell $(PKG_CONFIG)--cflags $(CHECK_PKG))
CHECK_LIBS   = $(shell $(PKG_CONFIG) --libs $(CHECK_PKG))

.PHONY: all build run valgrind leaks clean format test

all: build run

run: build
	$(BIN_DIR)/main

# ---------------- Main Program ----------------
build: $(OBJ_FILES) $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/main $(OBJ_FILES) $(LUA_LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(LUA_CFLAGS) -c $< -o $@

# ---------------- Test Build ----------------
test: $(OBJ_DIR) $(OBJ_FILES) $(TEST_OBJ_FILES) $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/run_tests $(LIB_OBJS) $(TEST_OBJ_FILES) $(CHECK_LIBS) $(LUA_LIBS)
	$(BIN_DIR)/run_tests

$(OBJ_DIR)/%.test.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) -c $< -o $@


# ---------------- Directories ----------------
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# ---------------- Tools ----------------------
valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BIN_DIR)/main

leaks: build
	./scripts/leaks.sh

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

format:
	clang-format -i $(SRC_FILES) $(HEAD_FILES)
