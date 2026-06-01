CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

SRC = src/main.c src/shell.c src/parser.c src/executor.c src/builtins.c src/redirection.c src/pipes.c src/background.c
TARGET = minishell

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)