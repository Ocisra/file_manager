FLAGS = -lncurses -lstdc++ -std=c++17 -g
SRC = $(wildcard *.cpp)

main: $(SRC)
	clang $^ -o $@ $(FLAGS)
