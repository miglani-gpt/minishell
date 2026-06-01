#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "shell.h"
#include "parser.h"
#include "executor.h"

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

    ssize_t characters_read = getline(&line, &buffer_size, stdin);

    if (characters_read == -1)
    {
        free(line);
        return NULL;
    }

    if (characters_read > 0 && line[characters_read - 1] == '\n')
    {
        line[characters_read - 1] = '\0';
    }

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
            printf("\n");
            break;
        }

        char **args = parse_input(line);

        execute_command(args);

        free(args);
        free(line);
    }
}