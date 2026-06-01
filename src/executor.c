#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "executor.h"
#include "builtins.h"

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

    if (handle_builtin(args))
    {
        return;
    }

    execute_external_command(args);
}