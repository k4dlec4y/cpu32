CC = gcc
CFLAGS = -std=c99 -c -Wall -Wextra -Iinclude

SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/cpu32

all: $(TARGET)

%: | build/

build/:
	mkdir -p $@

$(TARGET): $(BUILD_DIR)/cpu.o $(BUILD_DIR)/instructions.o $(BUILD_DIR)/main.o
	$(CC) $^ -o $@

$(BUILD_DIR)/cpu.o: $(SRC_DIR)/cpu.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/instructions.o: $(SRC_DIR)/instructions.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY = all clean
