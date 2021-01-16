FLAGS = -lncurses -std=c++20 -g
SRC = $(wildcard *.cpp)
CC = clang++

main: $(SRC)
	$(CC) $^ -o $@ $(FLAGS)
