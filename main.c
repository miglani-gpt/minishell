#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 100

void print_prompt(void)
{
    printf("minishell> ");
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

    // Remove trailing newline
    if (characters_read > 0 && line[characters_read - 1] == '\n')
    {
        line[characters_read - 1] = '\0';
    }

    return line;
}

char **parse_input(char *line)
{
    char **args = malloc(sizeof(char *) * MAX_ARGS);

    if (args == NULL)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    int index = 0;

    char *token = strtok(line, " ");

    while (token != NULL && index < MAX_ARGS - 1)
    {
        args[index] = token;
        index++;

        token = strtok(NULL, " ");
    }

    args[index] = NULL;

    return args;
}

void execute_command(char **args)
{
    if (args[0] == NULL)
    {
        return;
    }

    if (strcmp(args[0], "exit") == 0)
    {
        exit(0);
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return;
    }

    if (pid == 0)
    {
        // Child process
        if (execvp(args[0], args) == -1)
        {
            perror("minishell");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}

int main(void)
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

    return 0;
}