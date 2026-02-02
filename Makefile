CC = gcc
CFLAGS = -Wall -Wextra -O3

SRC_DIR = src
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)

TARGET = $(BIN_DIR)/car

all: $(TARGET)

# Create the bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(SRCS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

tcc: CC = tcc
tcc: $(TARGET)

clang: CC = clang
clang: $(TARGET)

win: CC = x86_64-w64-mingw32-gcc
win: $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f $(TARGET).exe
	rm -rf $(BIN_DIR)

.PHONY: all clean $(BIN_DIR)
