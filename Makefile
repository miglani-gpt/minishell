NAME := minishell

CC ?= gcc
CPPFLAGS := -Iinclude
CFLAGS ?= -Wall -Wextra -Werror -g
LDFLAGS ?=

SRC_DIR := src
OBJ_DIR := build
TEST_SCRIPT := tests/test_minishell.sh

SRCS := \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/shell.c \
	$(SRC_DIR)/lexer.c \
	$(SRC_DIR)/parser.c \
	$(SRC_DIR)/executor.c \
	$(SRC_DIR)/builtins.c \
	$(SRC_DIR)/redirection.c \
	$(SRC_DIR)/pipes.c \
	$(SRC_DIR)/background.c

OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean fclean re run test debug sanitize check help

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: all
	./$(NAME)

test: all
	bash ./$(TEST_SCRIPT) --no-build

debug: CFLAGS += -O0 -DDEBUG
debug: re

sanitize: CFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
sanitize: LDFLAGS += -fsanitize=address,undefined
sanitize: re

check: test

help:
	@echo "MiniShell build targets:"
	@echo "  make          Build $(NAME)"
	@echo "  make run      Build and run $(NAME)"
	@echo "  make test     Build and run automated tests"
	@echo "  make debug    Rebuild with debug macro enabled"
	@echo "  make sanitize Rebuild with AddressSanitizer/UBSan"
	@echo "  make clean    Remove object files"
	@echo "  make fclean   Remove object files and $(NAME)"
	@echo "  make re       Full rebuild"
