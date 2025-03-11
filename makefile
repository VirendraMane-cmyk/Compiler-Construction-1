# Compiler and flags
CC      = gcc
CFLAGS  = -w -g

# Source files and target executable
SRCS    = lexer.c keyword_table.c driver.c
OBJS    = $(SRCS:.c=.o)
TARGET  = lexer

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
