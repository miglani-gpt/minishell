#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "executor.h"
#include "builtins.h"
#include "redirection.h"

void execute_external_command(char **args)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("minishell: fork failed");
        return;
    }

    if (pid == 0)
    {
        if (setup_input_redirection(args, NULL) == -1)
        {
            exit(EXIT_FAILURE);
        }

        if (setup_output_redirection(args, NULL) == -1)
        {
            exit(EXIT_FAILURE);
        }

        if (execvp(args[0], args) == -1)
        {
            perror("minishell");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

void execute_command(char **args)
{
    if (args[0] == NULL)
    {
        return;
    }

    if (is_builtin(args[0]))
    {
        int saved_stdin = -1;
        int saved_stdout = -1;

        if (setup_input_redirection(args, &saved_stdin) == -1)
        {
            restore_input_redirection(saved_stdin);
            return;
        }

        if (setup_output_redirection(args, &saved_stdout) == -1)
        {
            restore_input_redirection(saved_stdin);
            restore_output_redirection(saved_stdout);
            return;
        }

        handle_builtin(args);

        restore_input_redirection(saved_stdin);
        restore_output_redirection(saved_stdout);

        return;
    }

    execute_external_command(args);
}