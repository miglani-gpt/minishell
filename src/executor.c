#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "executor.h"
#include "builtins.h"
#include "redirection.h"
#include "pipes.h"

void execute_external_command(t_command *command, int background)
{
    pid_t pid;

    pid = fork();
    if (pid < 0)
    {
        perror("minishell: fork failed");
        return;
    }

    if (pid == 0)
    {
        if (apply_redirections(command->redirections, NULL, NULL) == -1)
        {
            exit(EXIT_FAILURE);
        }

        if (execvp(command->argv[0], command->argv) == -1)
        {
            perror("minishell");
            exit(EXIT_FAILURE);
        }
    }

    if (background)
    {
        printf("[background pid: %d]\n", pid);
        waitpid(pid, NULL, WNOHANG);
    }
    else
    {
        int status;

        waitpid(pid, &status, 0);
    }
}

void execute_command(t_command *command, int background)
{
    if (command == NULL || command->argv == NULL || command->argv[0] == NULL)
    {
        return;
    }

    if (is_builtin(command->argv[0]))
    {
        int saved_stdin;
        int saved_stdout;

        if (background)
        {
            fprintf(stderr, "minishell: background execution not supported for built-ins\n");
            return;
        }

        if (apply_redirections(command->redirections, &saved_stdin, &saved_stdout) == -1)
        {
            restore_redirections(saved_stdin, saved_stdout);
            return;
        }

        handle_builtin(command->argv);
        fflush(stdout);
        restore_redirections(saved_stdin, saved_stdout);
        return;
    }

    execute_external_command(command, background);
}

void execute_parsed_input(t_parsed_input *parsed)
{
    if (parsed == NULL || parsed->commands == NULL)
    {
        return;
    }

    if (parsed->command_count == 1)
    {
        execute_command(parsed->commands, parsed->is_background);
        return;
    }

    if (parsed->is_background)
    {
        fprintf(stderr, "minishell: background execution not supported for pipelines yet\n");
        return;
    }

    if (parsed->command_count == 2)
    {
        execute_piped_commands(parsed->commands);
        return;
    }

    fprintf(stderr, "minishell: multiple pipes are not supported yet\n");
}
