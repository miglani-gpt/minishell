#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "pipes.h"
#include "redirection.h"

static void execute_left_pipe_child(t_command *command, int pipe_fd[2])
{
    close(pipe_fd[0]);

    if (dup2(pipe_fd[1], STDOUT_FILENO) < 0)
    {
        perror("minishell: dup2");
        close(pipe_fd[1]);
        exit(EXIT_FAILURE);
    }

    close(pipe_fd[1]);

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

static void execute_right_pipe_child(t_command *command, int pipe_fd[2])
{
    close(pipe_fd[1]);

    if (dup2(pipe_fd[0], STDIN_FILENO) < 0)
    {
        perror("minishell: dup2");
        close(pipe_fd[0]);
        exit(EXIT_FAILURE);
    }

    close(pipe_fd[0]);

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

void execute_piped_commands(t_command *commands)
{
    t_command *left_command;
    t_command *right_command;
    int pipe_fd[2];
    pid_t pid1;
    pid_t pid2;

    if (commands == NULL || commands->next == NULL)
    {
        return;
    }

    left_command = commands;
    right_command = commands->next;

    if (pipe(pipe_fd) == -1)
    {
        perror("minishell: pipe");
        return;
    }

    pid1 = fork();
    if (pid1 < 0)
    {
        perror("minishell: fork");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return;
    }

    if (pid1 == 0)
    {
        execute_left_pipe_child(left_command, pipe_fd);
    }

    pid2 = fork();
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
        execute_right_pipe_child(right_command, pipe_fd);
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}
