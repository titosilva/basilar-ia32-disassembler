# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Output binary
TARGET = main

# Find all source files
SRC_DIR = src
SRCS = $(shell find $(SRC_DIR) -name '*.c')
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJS) main.o
	$(CC) $(CFLAGS) -o $@ $(OBJS) main.o

debug: CFLAGS += -g
debug: CFLAGS += -DDEBUG
debug: $(TARGET)

# Compile the main.c file
main.o: main.c
	$(CC) $(CFLAGS) -c main.c

# Compile each .c file into .o file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean the build artifacts
clean:
	rm -f $(TARGET) $(OBJS) main.o

# Phony targets
.PHONY: all clean
