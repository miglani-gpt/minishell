#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "pipes.h"
#include "redirection.h"

int has_pipe(char **args)
{
    int i = 0;

    while (args[i] != NULL)
    {
        if (strcmp(args[i], "|") == 0)
        {
            return i;
        }

        i++;
    }

    return -1;
}

void execute_piped_command(char **args)
{
    int pipe_index = has_pipe(args);

    if (pipe_index == -1)
    {
        return;
    }

    if (pipe_index == 0 || args[pipe_index + 1] == NULL)
    {
        fprintf(stderr, "minishell: syntax error near unexpected token '|'\n");
        return;
    }

    args[pipe_index] = NULL;

    char **left_command = args;
    char **right_command = &args[pipe_index + 1];

    int pipe_fd[2];

    if (pipe(pipe_fd) == -1)
    {
        perror("minishell: pipe");
        return;
    }

    pid_t pid1 = fork();

    if (pid1 < 0)
    {
        perror("minishell: fork");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return;
    }

    if (pid1 == 0)
    {
        /*
         * Child 1:
         * Runs the left command.
         * Its stdout goes into the pipe.
         */

        close(pipe_fd[0]);

        if (dup2(pipe_fd[1], STDOUT_FILENO) < 0)
        {
            perror("minishell: dup2");
            close(pipe_fd[1]);
            exit(EXIT_FAILURE);
        }

        close(pipe_fd[1]);

        if (setup_input_redirection(left_command, NULL) == -1)
        {
            exit(EXIT_FAILURE);
        }

        if (execvp(left_command[0], left_command) == -1)
        {
            perror("minishell");
            exit(EXIT_FAILURE);
        }
    }

    pid_t pid2 = fork();

    if (pid2 < 0)
    {
        perror("minishell: fork");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        waitpid(pid1, NULL, 0);
        return;
    }

    if (pid2 == 0)
    {
        /*
         * Child 2:
         * Runs the right command.
         * Its stdin comes from the pipe.
         */

        close(pipe_fd[1]);

        if (dup2(pipe_fd[0], STDIN_FILENO) < 0)
        {
            perror("minishell: dup2");
            close(pipe_fd[0]);
            exit(EXIT_FAILURE);
        }

        close(pipe_fd[0]);

        if (setup_output_redirection(right_command, NULL) == -1)
        {
            exit(EXIT_FAILURE);
        }

        if (execvp(right_command[0], right_command) == -1)
        {
            perror("minishell");
            exit(EXIT_FAILURE);
        }
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}