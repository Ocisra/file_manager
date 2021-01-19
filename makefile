COMPILE_FLAGS = -lncurses -std=c++20
DEBUG_FLAGS = -g -Wall -Wextra -pedantic
FLAGS = $(COMPILE_FLAGS) $(DEBUG_FLAGS)
SRC = $(wildcard *.cpp)
BIN_DIR = bin/
CC = clang++

main: $(SRC)
	$(CC) $^ -o $(BIN_DIR)$@ $(FLAGS)
