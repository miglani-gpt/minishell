#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "redirection.h"

static void remove_redirection_tokens(char **args, int index)
{
    int j = index;

    while (args[j + 2] != NULL)
    {
        args[j] = args[j + 2];
        j++;
    }

    args[j] = NULL;
}

int setup_output_redirection(char **args, int *saved_stdout)
{
    int i = 0;

    while (args[i] != NULL)
    {
        if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0)
        {
            int flags;

            if (args[i + 1] == NULL)
            {
                fprintf(stderr, "minishell: syntax error: expected file after '%s'\n", args[i]);
                return -1;
            }

            if (strcmp(args[i], ">") == 0)
            {
                flags = O_WRONLY | O_CREAT | O_TRUNC;
            }
            else
            {
                flags = O_WRONLY | O_CREAT | O_APPEND;
            }

            int fd = open(args[i + 1], flags, 0644);

            if (fd < 0)
            {
                perror("minishell: open");
                return -1;
            }

            if (saved_stdout != NULL)
            {
                *saved_stdout = dup(STDOUT_FILENO);

                if (*saved_stdout < 0)
                {
                    perror("minishell: dup");
                    close(fd);
                    return -1;
                }
            }

            if (dup2(fd, STDOUT_FILENO) < 0)
            {
                perror("minishell: dup2");
                close(fd);
                return -1;
            }

            close(fd);

            remove_redirection_tokens(args, i);

            return 1;
        }

        i++;
    }

    return 0;
}

void restore_output_redirection(int saved_stdout)
{
    if (saved_stdout >= 0)
    {
        if (dup2(saved_stdout, STDOUT_FILENO) < 0)
        {
            perror("minishell: restore stdout");
        }

        close(saved_stdout);
    }
}

int setup_input_redirection(char **args, int *saved_stdin)
{
    int i = 0;

    while (args[i] != NULL)
    {
        if (strcmp(args[i], "<") == 0)
        {
            if (args[i + 1] == NULL)
            {
                fprintf(stderr, "minishell: syntax error: expected file after '<'\n");
                return -1;
            }

            int fd = open(args[i + 1], O_RDONLY);

            if (fd < 0)
            {
                perror("minishell: open");
                return -1;
            }

            if (saved_stdin != NULL)
            {
                *saved_stdin = dup(STDIN_FILENO);

                if (*saved_stdin < 0)
                {
                    perror("minishell: dup");
                    close(fd);
                    return -1;
                }
            }

            if (dup2(fd, STDIN_FILENO) < 0)
            {
                perror("minishell: dup2");
                close(fd);
                return -1;
            }

            close(fd);

            remove_redirection_tokens(args, i);

            return 1;
        }

        i++;
    }

    return 0;
}

void restore_input_redirection(int saved_stdin)
{
    if (saved_stdin >= 0)
    {
        if (dup2(saved_stdin, STDIN_FILENO) < 0)
        {
            perror("minishell: restore stdin");
        }

        close(saved_stdin);
    }
}