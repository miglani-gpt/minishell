CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = minishell

all:
	$(CC) $(CFLAGS) main.c -o $(TARGET)

clean:
	rm -f $(TARGET)