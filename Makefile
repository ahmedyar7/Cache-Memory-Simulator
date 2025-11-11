# Makefile for the Cache Simulator

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
LDFLAGS = -lm

# Executable name
TARGET = cache_simulator

# Source files
SRCS = main.c cache.c utils.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default rule: build the target
all: $(TARGET)

# Rule to link the target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Rule to compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule: remove built files
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean