#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shell.h"
#include "parser.h"
#include "executor.h"

static int is_shell_whitespace(char character)
{
    return character == ' ' || character == '\t' || character == '\n' ||
           character == '\r' || character == '\v' || character == '\f';
}

static void trim_input(char *line)
{
    size_t start = 0;
    size_t end;
    size_t new_length;

    if (line == NULL)
    {
        return;
    }

    while (line[start] != '\0' && is_shell_whitespace(line[start]))
    {
        start++;
    }

    if (start > 0)
    {
        memmove(line, line + start, strlen(line + start) + 1);
    }

    end = strlen(line);

    while (end > 0 && is_shell_whitespace(line[end - 1]))
    {
        end--;
    }

    new_length = end;
    line[new_length] = '\0';
}

static int is_empty_input(const char *line)
{
    if (line == NULL)
    {
        return 1;
    }

    while (*line != '\0')
    {
        if (!is_shell_whitespace(*line))
        {
            return 0;
        }

        line++;
    }

    return 1;
}

void print_prompt(void)
{
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("minishell:%s> ", cwd);
    }
    else
    {
        printf("minishell> ");
    }

    fflush(stdout);
}

char *read_input(void)
{
    char *line = NULL;
    size_t buffer_size = 0;
    ssize_t characters_read;

    errno = 0;
    characters_read = getline(&line, &buffer_size, stdin);

    if (characters_read == -1)
    {
        if (!feof(stdin))
        {
            perror("minishell: getline");
        }

        free(line);
        return NULL;
    }

    while (characters_read > 0 &&
           (line[characters_read - 1] == '\n' || line[characters_read - 1] == '\r'))
    {
        line[characters_read - 1] = '\0';
        characters_read--;
    }

    trim_input(line);

    return line;
}

void shell_loop(void)
{
    while (1)
    {
        print_prompt();

        char *line = read_input();

        if (line == NULL)
        {
            printf("exit\n");
            break;
        }

        if (is_empty_input(line))
        {
            free(line);
            continue;
        }

        char **args = parse_input(line);

        execute_command(args);

        free(args);
        free(line);
    }
}
