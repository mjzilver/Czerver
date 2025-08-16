SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
HEAD_FILES = $(wildcard $(SRC_DIR)/*.h)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC = gcc
CFLAGS = -Wall -g

.PHONY: all build run valgrind leaks clean format

all: build run

build: $(OBJ_FILES)
	mkdir -p $(BIN_DIR) $(OBJ_DIR)
	$(CC) -o $(BIN_DIR)/main $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

run: build
	$(BIN_DIR)/main

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BIN_DIR)/main

leaks: build
	./scripts/leaks.sh

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

format:
	clang-format -i $(SRC_FILES) $(HEAD_FILES)

