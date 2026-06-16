#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "redirection.h"

static int save_descriptor_once(int descriptor, int *saved_descriptor)
{
    if (saved_descriptor == NULL || *saved_descriptor >= 0)
    {
        return 0;
    }

    *saved_descriptor = dup(descriptor);
    if (*saved_descriptor < 0)
    {
        perror("minishell: dup");
        return -1;
    }

    return 0;
}

static int open_redirection_file(t_redirection *redirection)
{
    int flags;

    if (redirection->type == REDIR_INPUT)
    {
        return open(redirection->filename, O_RDONLY);
    }

    if (redirection->type == REDIR_APPEND)
    {
        flags = O_WRONLY | O_CREAT | O_APPEND;
    }
    else
    {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    }

    return open(redirection->filename, flags, 0644);
}

static int apply_single_redirection(t_redirection *redirection,
                                    int *saved_stdin,
                                    int *saved_stdout)
{
    int fd;
    int target_fd;

    fd = open_redirection_file(redirection);
    if (fd < 0)
    {
        perror("minishell: open");
        return -1;
    }

    if (redirection->type == REDIR_INPUT)
    {
        target_fd = STDIN_FILENO;
        if (save_descriptor_once(STDIN_FILENO, saved_stdin) == -1)
        {
            close(fd);
            return -1;
        }
    }
    else
    {
        target_fd = STDOUT_FILENO;
        if (save_descriptor_once(STDOUT_FILENO, saved_stdout) == -1)
        {
            close(fd);
            return -1;
        }
    }

    if (dup2(fd, target_fd) < 0)
    {
        perror("minishell: dup2");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int apply_redirections(t_redirection *redirections, int *saved_stdin, int *saved_stdout)
{
    if (saved_stdin != NULL)
    {
        *saved_stdin = -1;
    }
    if (saved_stdout != NULL)
    {
        *saved_stdout = -1;
    }

    while (redirections != NULL)
    {
        if (apply_single_redirection(redirections, saved_stdin, saved_stdout) == -1)
        {
            return -1;
        }

        redirections = redirections->next;
    }

    return 0;
}

void restore_redirections(int saved_stdin, int saved_stdout)
{
    if (saved_stdin >= 0)
    {
        if (dup2(saved_stdin, STDIN_FILENO) < 0)
        {
            perror("minishell: restore stdin");
        }
        close(saved_stdin);
    }

    if (saved_stdout >= 0)
    {
        if (dup2(saved_stdout, STDOUT_FILENO) < 0)
        {
            perror("minishell: restore stdout");
        }
        close(saved_stdout);
    }
}
