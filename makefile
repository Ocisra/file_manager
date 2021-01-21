CC = clang++
LDFLAGS = -lncurses
COMPILE_FLAGS = -std=c++20 -MMD -MP
DEBUG_FLAGS = -g -Wall -Wextra -pedantic
FLAGS = $(COMPILE_FLAGS) $(DEBUG_FLAGS)

BIN_DIR = bin/
SRC_DIR = ./
BUILD_DIR = build/

SRC = $(wildcard $(SRC_DIR)*.cpp)
OBJ = $(SRC:$(SRC_DIR)%=$(BUILD_DIR)%.o)
DEP = $(OBJ:.o=.d)


$(BIN_DIR)main: $(OBJ)
	echo built $@
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)%.cpp.o: %.cpp
	echo built $@
	$(CC) -c $< -o $@ $(FLAGS)

%/:
	mkdir $@

.PHONY: clean init

init: $(BUILD_DIR) $(BIN_DIR)

clean:
	rm -r $(BUILD_DIR)
	rm -r $(BIN_DIR)

-include $(DEP)
