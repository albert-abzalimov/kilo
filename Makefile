# Makefile for the Kilo text editor

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
# List all your source files
SRCS = kilo.c terminal.c input.c append_buffer.c output.c
# Automatically generate object file names from source files
OBJS = $(SRCS:.c=.o)
# The name of your executable
TARGET = kilo

# Rule to build the final executable 'kilo'
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)
# Generic rule to compile any .c file into a .o file
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean up the generated files
clean:
	rm -f $(OBJS) $(TARGET)
